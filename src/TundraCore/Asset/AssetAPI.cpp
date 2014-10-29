// For conditions of distribution and use, see copyright notice in LICENSE

#include "StableHeaders.h"

#include "AssetAPI.h"
#include "CoreDefines.h"
#include "IAssetTransfer.h"
#include "IAsset.h"
#include "IAssetBundle.h"
#include "IAssetStorage.h"
#include "IAssetProvider.h"
#include "IAssetTypeFactory.h"
#include "IAssetBundleTypeFactory.h"
#include "IAssetUploadTransfer.h"
#include "GenericAssetFactory.h"
#include "NullAssetFactory.h"
#include "LocalAssetProvider.h"

#include "Framework.h"
#include "LoggingFunctions.h"
#include "CoreStringUtils.h"

#include <Context.h>
#include <Profiler.h>
#include <StringUtils.h>
#include <FileSystem.h>
#include <File.h>

namespace Tundra
{

AssetAPI::AssetAPI(Framework *framework, bool headless) :
    Object(framework->GetContext()),
    fw(framework),
    isHeadless(headless),
    assetCache(0)
{
    AssetProviderPtr local(new LocalAssetProvider(fw));
    RegisterAssetProvider(local);

    // The Asset API always understands at least this single built-in asset type "Binary".
    // You can use this type to request asset data as binary, without generating any kind of in-memory representation or loading for it.
    // Your module/component can then parse the content in a custom way.
    RegisterAssetTypeFactory(AssetTypeFactoryPtr(new BinaryAssetFactory("Binary", "")));

    if (fw->HasCommandLineParameter("--accept_unknown_http_sources"))
        LogWarning("--accept_unknown_http_sources: this format of the command-line parameter is deprecated and support for it will be removed. Use --acceptUnknownHttpSources instead.");
    if (fw->HasCommandLineParameter("--disable_http_ifmodifiedsince"))
        LogWarning("--disable_http_ifmodifiedsince: this format of the command-line parameter is deprecated and support for it will be removed. Use --disableHttpIfModifiedSince instead.");
    if (fw->HasCommandLineParameter("--accept_unknown_local_sources"))
        LogWarning("--accept_unknown_local_sources: this format of the command-line parameter is deprecated and support for it will be removed. Use --acceptUnknownLocalSources instead.");
    if (fw->HasCommandLineParameter("--no_async_asset_load"))
        LogWarning("--no_async_asset_load: this format of the command-line parameter is deprecated and support for it will be removed. Use --noAsyncAssetLoad instead.");
    if (fw->HasCommandLineParameter("--clear-asset-cache"))
        LogWarning("--clear-asset-cache: this format of the command-line parameter is deprecated and support for it will be removed. Use --clearAssetCache instead.");
}

AssetAPI::~AssetAPI()
{
    Reset();
}

void AssetAPI::OpenAssetCache(String /*directory*/)
{
    /// \todo Implement AssetCache
    //SAFE_DELETE(assetCache);
    // assetCache = new AssetCache(this, directory);
}

Vector<AssetProviderPtr> AssetAPI::AssetProviders() const
{
    return providers;
}

void AssetAPI::RegisterAssetProvider(AssetProviderPtr provider)
{
    for(uint i = 0; i < providers.Size(); ++i)
    {
        if (providers[i]->Name() == provider->Name())
        {
            LogWarning("AssetAPI::RegisterAssetProvider: Provider with name '" + provider->Name() + "' already registered.");
            return;
        }
    }
    providers.Push(provider);
}

AssetStoragePtr AssetAPI::AssetStorageByName(const String &name) const
{
    foreach(const AssetProviderPtr &provider, providers)
    {
        Vector<AssetStoragePtr> storages = provider->Storages();
        foreach(AssetStoragePtr storage, storages)
            if (storage->Name().Compare(name, false) == 0)
                return storage;
    }
    return AssetStoragePtr();
}

AssetStoragePtr AssetAPI::StorageForAssetRef(const String &ref) const
{
    PROFILE(AssetAPI_StorageForAssetRef);
    foreach(const AssetProviderPtr &provider, providers)
    {
        AssetStoragePtr storage = provider->StorageForAssetRef(ref);
        if (storage)
            return storage;
    }
    return AssetStoragePtr();
}

bool AssetAPI::RemoveAssetStorage(const String &name)
{
    ///\bug Currently it is possible to have e.g. a local storage with name "Foo" and a http storage with name "Foo", and it will
    /// not be possible to specify which storage to delete.
    foreach(const AssetProviderPtr &provider, providers)
        if (provider->RemoveAssetStorage(name))
            return true;

    return false;
}

AssetStoragePtr AssetAPI::DeserializeAssetStorageFromString(const String &storage, bool fromNetwork)
{
    for(uint i = 0; i < providers.Size(); ++i)
    {
        AssetStoragePtr assetStorage = providers[i]->TryDeserializeStorageFromString(storage, fromNetwork);
        // The above function will call back to AssetAPI::EmitAssetStorageAdded.
        if (assetStorage)
        {
            // Make this storage the default storage if it was requested so.
            HashMap<String, String> s = AssetAPI::ParseAssetStorageString(storage);
            if (s.Contains("default") && Urho3D::ToBool(s["default"]))
                SetDefaultAssetStorage(assetStorage);
            return assetStorage;
        }
    }
    return AssetStoragePtr();
}

AssetStoragePtr AssetAPI::DefaultAssetStorage() const
{
    AssetStoragePtr defStorage = defaultStorage.Lock();
    if (defStorage)
        return defStorage;

    // If no default storage set, return the first one on the list.
    Vector<AssetStoragePtr> storages = AssetStorages();
    if (storages.Size() > 0)
        return storages[0];

    return AssetStoragePtr();
}

void AssetAPI::SetDefaultAssetStorage(const AssetStoragePtr &storage)
{
    defaultStorage = storage;
    if (storage)
        LogInfo("Set asset storage \"" + storage->Name() + "\" as the default storage (" + storage->SerializeToString() + ").");
    else
        LogInfo("Set (null) as the default asset storage.");
}

AssetMap AssetAPI::AssetsOfType(const String& type) const
{
    AssetMap ret;
    for(AssetMap::const_iterator i = assets.begin(); i != assets.end(); ++i)
        if (i->second->Type().Compare(type, false) == 0)
            ret[i->first] = i->second;
    return ret;
}

Vector<AssetStoragePtr> AssetAPI::AssetStorages() const
{
    Vector<AssetStoragePtr> storages;

    Vector<AssetProviderPtr> providers = AssetProviders();
    for(uint i = 0; i < providers.Size(); ++i)
    {
        Vector<AssetStoragePtr> stores = providers[i]->Storages();
        storages.Push(stores);
    }

    return storages;
}

AssetAPI::FileQueryResult AssetAPI::ResolveLocalAssetPath(String ref, String baseDirectoryContext, String &outFilePath, String *subAssetName) const
{
    Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();

    // Make sure relative paths are turned into local paths.
    String refLocal = ResolveAssetRef("local://" + GuaranteeTrailingSlash(baseDirectoryContext), ref);

    String pathFilename;
    String protocol;
    AssetRefType sourceRefType = ParseAssetRef(refLocal, &protocol, 0, 0, 0, &pathFilename, 0, 0, subAssetName);
    if (sourceRefType == AssetRefInvalid || sourceRefType == AssetRefExternalUrl || sourceRefType == AssetRefNamedStorage)
    {
        // Failed to resolve path, it is not local.
        outFilePath = refLocal; // Return the path that was tried.
        return FileQueryExternalFile;
    }
    else if (sourceRefType == AssetRefLocalPath)
    {
        outFilePath = pathFilename;
        return fileSystem->FileExists(outFilePath) ? FileQueryLocalFileFound : FileQueryLocalFileMissing;
    }
    else if (sourceRefType == AssetRefLocalUrl)
    {
        outFilePath = pathFilename;
        if (fileSystem->FileExists(outFilePath))
            return FileQueryLocalFileFound;

        outFilePath = RecursiveFindFile(baseDirectoryContext, pathFilename);
        if (!outFilePath.Empty())
            return FileQueryLocalFileFound;

        ///\todo Query all local storages for the file.
        ///\todo Can't currently query the LocalAssetProviders here directly (wrong direction for dependency chain).

        outFilePath = ref;
        return FileQueryLocalFileMissing;
    }
    else
    {
        // Unknown reference type.
        outFilePath = ref;
        return FileQueryExternalFile;
    }
}

AssetAPI::AssetRefType AssetAPI::ParseAssetRef(String assetRef, String *outProtocolPart, String *outNamedStorage, String *outProtocol_Path, 
                                               String *outPath_Filename_SubAssetName, String *outPath_Filename, String *outPath, 
                                               String *outFilename, String *outSubAssetName, String *outFullRef, String *outFullRefNoSubAssetName)
{
    if (outProtocolPart) *outProtocolPart = "";
    if (outNamedStorage) *outNamedStorage = "";
    if (outProtocol_Path) *outProtocol_Path = "";
    String protocol_path = "";
    if (outPath_Filename_SubAssetName) *outPath_Filename_SubAssetName = "";
    if (outPath_Filename) *outPath_Filename = "";
    if (outPath) *outPath = "";
    if (outFilename) *outFilename = "";
    if (outSubAssetName) *outSubAssetName = "";
    if (outFullRef) *outFullRef = "";
    if (outFullRefNoSubAssetName) *outFullRefNoSubAssetName = "";

    /* Examples of asset refs:

     a1) local://asset.sfx                                      AssetRefType = AssetRefLocalUrl.
         file:///unix/file/path/asset.sfx
         file://C:/windows/file/path/asset.sfx

     a2) http://server.com/asset.sfx                            AssetRefType = AssetRefExternalUrl.
         someotherprotocol://some/path/specifier/asset.sfx

     b0) special case: www.server.com/asset.png                 AssetRefType = AssetRefExternalUrl with 'http' hardcoded as protocol.
         As customary with web browsers, people expect to be able to write just www.server.com/asset.png when they mean a network URL.
         We detect this case as a string that starts with 'www.'.

     b1) /unix/absolute/path/asset.sfx                          AssetRefType = AssetRefLocalPath.

     b2) X:/windows/forwardslash/path/asset.sfx                 AssetRefType = AssetRefLocalPath.
         X:\windows\backslash\path\asset.sfx
         X:\windows/mixedupslash\path/asset.sfx

     b3) namedstorage:asset.sfx                                 AssetRefType = AssetRefNamedStorage.

     b4) asset.sfx                                              AssetRefType = AssetRefRelativePath.
         ./asset.sfx
         .\asset.sfx
         relative/path/asset.sfx
         ./relative/path/asset.sfx
         ../relative/path/asset.sfx
         
        Each category can have an optional extra subasset specifier, separated with a comma. Examples:

             asset.sfx,subAssetName.sfx2
             asset.sfx,subAssetNameWithoutSuffix
             asset.sfx, "subAssetName as a string with spaces"
             http://server.com/asset.sfx,subAssetName.sfx2
    */

    assetRef = assetRef.Trimmed();
    assetRef.Replace('\\', '/'); // Normalize all path separators to use forward slashes.

    String fullPath; // Contains the url without the "protocolPart://" prefix.
    AssetRefType refType = AssetRefInvalid;
    unsigned pos;
    if ((pos = assetRef.Find("://")) != String::NPOS) // Is ref of type 'a)' above?
    {
        String protocol = assetRef.Substring(0, pos);
        if (protocol.Compare("local", false) == 0 || protocol.Compare("file", false) == 0)
            refType = AssetRefLocalUrl;
        else
            refType = AssetRefExternalUrl;
        if (outProtocolPart)
            *outProtocolPart = protocol;

        fullPath = assetRef.Substring(pos + 3);
        protocol_path = protocol + "://";
        if (outProtocol_Path) // Partially save the beginning of the protocol & path part. This will be completed below with the full path.
            *outProtocol_Path = protocol_path;

        if (outFullRef)
            *outFullRef = protocol.ToLower() + "://";
    }
    else if (assetRef.StartsWith("www.", false)) // Ref is of type 'b0)'?
    {
        refType = AssetRefExternalUrl;
        if (outProtocolPart)
            *outProtocolPart = "http";
        fullPath = assetRef;

        protocol_path = "http://";
        if (outProtocol_Path) // Partially save the beginning of the protocol & path part. This will be completed below with the full path.
            *outProtocol_Path = protocol_path;
        if (outFullRef)
            *outFullRef = "http://";
    }
    else if (assetRef.StartsWith("/")) // Is ref of type 'b1)'?
    {
        refType = AssetRefLocalPath;
        fullPath = assetRef;
    }
    else if ((pos = assetRef.Find(":/")) != String::NPOS)
    {
        refType = AssetRefLocalPath;
        fullPath = assetRef;
    }
    else if ((pos = assetRef.Find(':')) != String::NPOS)
    {
        refType = AssetRefNamedStorage;
        String storage = assetRef.Substring(0, pos);
        if (outNamedStorage)
            *outNamedStorage = storage;
        fullPath = assetRef.Substring(pos + 1);

        protocol_path = storage + ":";
        if (outProtocol_Path)
            *outProtocol_Path = protocol_path;
        if (outFullRef)
            *outFullRef = protocol_path;
    }
    else // We assume it must be of type b4).
    {
        refType = AssetRefRelativePath;
        fullPath = assetRef;
    }

    // After the above check, we are left with a fullPath reference that can only contain three parts: directory, local name and subAssetName.
    // The protocol specifier or named storage part has been stripped off.
    if (outPath_Filename_SubAssetName)
        *outPath_Filename_SubAssetName = fullPath.Trimmed();

    // Parse subAssetName if it exists.
    String subAssetName = "";

    pos = fullPath.Find(',');
    if (pos == String::NPOS)
        pos = fullPath.Find('#');
    
    if (pos != String::NPOS)
    {
        String assetRef = fullPath.Substring(0, pos);
        subAssetName = fullPath.Substring(pos + 1).Trimmed();
        if (subAssetName.StartsWith("\""))
            subAssetName = subAssetName.Substring(1);
        if (subAssetName.EndsWith("\""))
            subAssetName = subAssetName.Substring(0, subAssetName.Length() - 1);
        if (outSubAssetName)
            *outSubAssetName = subAssetName;
        fullPath = assetRef; // Remove the subAssetName from the asset ref so that the parsing can continue without the subAssetName in it.
    }

    if (outPath_Filename) 
        *outPath_Filename = fullPath;

    // Now the only thing that is left is to split the base filename and the path for the asset.
    unsigned lastPeriodIndex = fullPath.FindLast('.');
    unsigned directorySeparatorIndex = fullPath.FindLast('/');
    if (lastPeriodIndex == String::NPOS || lastPeriodIndex < directorySeparatorIndex)
    {
        /** Don't do anything, use the last dir separator as is. The old code below
            breaks refs that do not have a asset extension (eg. .mesh) in the filename part.

            We want the following to work correctly.
            http://myservice.com/list/objects?id=15
                -> Path     : http://myservice.com/list/
                -> Filename : objects?id=15
            http://asset.service.com/some/path/assetWithoutExtension
                -> Path     : http://asset.service.com/some/path/
                -> Filename : assetWithoutExtension

            @note Not having suffix in the asset filename will only work for "Binary"
            requests. As the file suffix is the main and only way we detect what IAsset
            implementation should handle the incoming data. */

        //directorySeparatorIndex = fullPathRef.length()-1;
    }

    String path = GuaranteeTrailingSlash(fullPath.Substring(0, directorySeparatorIndex+1).Trimmed());
    if (outPath)
        *outPath = path;
    protocol_path += path;
    if (outProtocol_Path)
        *outProtocol_Path += path;

    /** @todo Handle url query separately? outFilename will have the query (and should as the request should be done with it)
        but if something is interested in the query alone, should be parsed separately to a new out String* param.
        Note however that this will only work for "Binary" type assets, if you have <ref>/mymesh.mesh?something=x the
        file suffix will be read incorrectly and not passed to the correct IAsset implementation. */
    String assetFilename = fullPath.Substring(directorySeparatorIndex+1);
    if (outFilename)
        *outFilename = assetFilename;
    if (outFullRef)
    {
        *outFullRef += fullPath;
        if (!subAssetName.Empty())
        {
            if (subAssetName.Contains(' '))
                *outFullRef += "#\"" + subAssetName + "\"";
            else
                *outFullRef += "#" + subAssetName;
        }
    }

    if (outFullRefNoSubAssetName)
        *outFullRefNoSubAssetName = GuaranteeTrailingSlash(protocol_path) + assetFilename;
    return refType;
}

String AssetAPI::ExtractFilenameFromAssetRef(String ref)
{
    String filename;
    ParseAssetRef(ref, 0, 0, 0, 0, 0, 0, &filename, 0);
    return filename;
}

String AssetAPI::RecursiveFindFile(String basePath, String filename)
{
    Vector<String> results;
    // Nasty hack to access Urho filesystem subsystem statically
    Urho3D::FileSystem* fileSystem = Framework::Instance()->GetSubsystem<Urho3D::FileSystem>();
    fileSystem->ScanDir(results, basePath, "*.*", Urho3D::SCAN_FILES, true);

    foreach(String result, results)
    {
        if (result.EndsWith(filename, false))
            return GuaranteeTrailingSlash(basePath) + result;
    }

    return "";
}

AssetPtr AssetAPI::CreateAssetFromFile(String assetType, String assetFile)
{
    AssetPtr asset = CreateNewAsset(assetType, assetFile);
    if (!asset)
        return AssetPtr();
    bool success = asset->LoadFromFile(assetFile);
    if (success)
        return asset;
    else
    {
        ForgetAsset(asset, false);
        return AssetPtr();
    }
}

bool AssetAPI::ForgetAsset(String assetRef, bool removeDiskSource)
{
    return ForgetAsset(FindAsset(assetRef), removeDiskSource);
}

bool AssetAPI::ForgetAsset(AssetPtr asset, bool removeDiskSource)
{
    if (!asset.Get())
        return false;

    AssetAboutToBeRemoved.Emit(asset);

    // If we are supposed to remove the cached (or original for local assets) version of the asset, do so.
    if (removeDiskSource && !asset->DiskSource().Empty())
    {
        DiskSourceAboutToBeRemoved.Emit(asset);
        // Remove disk watcher before deleting the file. Otherwise we get tons of spam and not wanted reload tries.
        /*
        /// \todo Implement DiskSourceChangeWatcher & AssetCache
        if (diskSourceChangeWatcher)
            diskSourceChangeWatcher->removePath(asset->DiskSource());
        if (assetCache)
            assetCache->DeleteAsset(asset->Name());
        */
        asset->SetDiskSource("");
    }

    // Do an explicit unload of the asset before deletion (the dtor of each asset has to do unload as well, but this handles the cases where
    // some object left a dangling strong ref to an asset).
    asset->Unload();

    // Remove any pending transfers for this asset.
    AssetTransferMap::iterator transferIter = FindTransferIterator(asset->Name());
    if (transferIter != currentTransfers.end())
        currentTransfers.erase(transferIter);

    // Remove the asset from internal state.
    AssetMap::iterator iter = assets.find(asset->Name());
    if (iter == assets.end())
    {
        LogError("AssetAPI::ForgetAsset called on asset \"" + asset->Name() + "\", which does not exist in AssetAPI!");
        return false;
    }

    // Remove file from disk watcher.
    /*
    if (diskSourceChangeWatcher && !asset->DiskSource().isEmpty())
        diskSourceChangeWatcher->removePath(asset->DiskSource());
    */
    assets.erase(iter);
    return true;
}

bool AssetAPI::ForgetBundle(String bundleRef, bool removeDiskSource)
{
    return ForgetBundle(FindBundle(bundleRef), removeDiskSource);
}

bool AssetAPI::ForgetBundle(AssetBundlePtr bundle, bool removeDiskSource)
{
    if (!bundle.Get())
        return false;

    AssetBundleAboutToBeRemoved.Emit(bundle);

    // If we are supposed to remove the cached (or original for local assets) version of the asset, do so.
    if (removeDiskSource && !bundle->DiskSource().Empty())
    {
        BundleDiskSourceAboutToBeRemoved.Emit(bundle);

        // Remove disk watcher before deleting the file. Otherwise we get tons of spam and not wanted reload tries.
        /*
        if (diskSourceChangeWatcher)
            diskSourceChangeWatcher->removePath(bundle->DiskSource());
        if (assetCache)
            assetCache->DeleteAsset(bundle->Name());
        */
        bundle->SetDiskSource("");
    }

    // Do an explicit unload of the asset before deletion (the dtor of each asset has to do unload as well, but this handles the cases where
    // some object left a dangling strong ref to an asset).
    bundle->Unload();

    AssetBundleMap::iterator iter = assetBundles.find(bundle->Name());
    if (iter == assetBundles.end())
    {
        LogError("AssetAPI::ForgetBundle called on asset \"" + bundle->Name() + "\", which does not exist in AssetAPI!");
        return false;
    }
    /*
    if (diskSourceChangeWatcher && !bundle->DiskSource().Empty())
        diskSourceChangeWatcher->removePath(bundle->DiskSource());
    */
    assetBundles.erase(iter);
    return true;
}

void AssetAPI::DeleteAssetFromStorage(String assetRef)
{
    AssetPtr asset = FindAsset(assetRef);

    AssetProviderPtr provider = (asset.Get() ? asset->AssetProvider() : AssetProviderPtr());
    if (!provider)
        provider = ProviderForAssetRef(assetRef); // If the actual AssetPtr didn't specify the originating provider, try to guess it from the assetRef string.

    // We're erasing the asset from the storage, so also clean it from memory and disk source to avoid any leftovers from remaining in the system.
    if (asset)
        ForgetAsset(asset, true);

    if (!provider)
    {
        LogError("AssetAPI::DeleteAssetFromStorage called on asset \"" + assetRef + "\", but the originating provider was not set!");
        // Remove this asset from memory and from the disk source, the best we can do for it.
        return;
    }

    provider->DeleteAssetFromStorage(assetRef);
}

AssetUploadTransferPtr AssetAPI::UploadAssetFromFile(const String &filename, const String &storageName, const String &assetName)
{
    Urho3D::File file(GetContext(), filename, Urho3D::FILE_READ);

    if (!file.IsOpen())
    {
        LogError("AssetAPI::UploadAssetFromFile failed! File location not valid for " + filename);
        return AssetUploadTransferPtr();
    }
    String newAssetName = assetName;
    if (newAssetName.Empty())
        newAssetName = file.GetName().Split('/').Back();
    AssetStoragePtr storage = AssetStorageByName(storageName);
    if (!storage.Get())
    {
        LogError("AssetAPI::UploadAssetFromFile failed! No storage found with name " + storageName + "! Please add a storage with this name.");
        return AssetUploadTransferPtr();
    }
    
    AssetUploadTransferPtr transfer;
    transfer = UploadAssetFromFile(filename, storage, newAssetName);
    if (!transfer)
    {
        LogError("AssetAPI::UploadAssetFromFile failed!");
    }
    return transfer;
}

AssetUploadTransferPtr AssetAPI::UploadAssetFromFile(const String &filename, AssetStoragePtr destination, const String &assetName)
{
    if (filename.Empty())
    {
        LogError("AssetAPI::UploadAssetFromFile failed! No source filename given!");
        return AssetUploadTransferPtr();
    }

    if (assetName.Empty())
    {
        LogError("AssetAPI::UploadAssetFromFile failed! No destination asset name given!");
        return AssetUploadTransferPtr();
    }
    if (!destination)
    {
        LogError("AssetAPI::UploadAssetFromFile failed! The passed destination asset storage was null!");
        return AssetUploadTransferPtr();
    }
    AssetProviderPtr provider = destination->provider.Lock();
    if (!provider)
    {
        LogError("AssetAPI::UploadAssetFromFile failed! The provider pointer of the passed destination asset storage was null!");
        return AssetUploadTransferPtr();
    }
    Vector<u8> data;
    bool success = LoadFileToVector(filename, data);
    if (!success)
    {
        LogError("AssetAPI::UploadAssetFromFile failed! Could not load file to a data vector.");
        return AssetUploadTransferPtr();
    }
    if (data.Size() == 0)
    {
        LogError("AssetAPI::UploadAssetFromFile failed! Loaded file to data vector but size is zero.");
        return AssetUploadTransferPtr();
    }
    return UploadAssetFromFileInMemory(&data[0], data.Size(), destination, assetName);
}

AssetUploadTransferPtr AssetAPI::UploadAssetFromFileInMemory(const u8 *data, uint numBytes, AssetStoragePtr destination, const String &assetName)
{
    if (!data || numBytes == 0)
    {
        LogError("AssetAPI::UploadAssetFromFileInMemory failed! Null source data passed!");
        return AssetUploadTransferPtr();
    }

    if (assetName.Empty())
    {
        LogError("AssetAPI::UploadAssetFromFileInMemory failed! No destination asset name given!");
        return AssetUploadTransferPtr();
    }

    if (!destination)
    {
        LogError("AssetAPI::UploadAssetFromFileInMemory failed! The passed destination asset storage was null!");
        return AssetUploadTransferPtr();
    }

    if (!destination->Writable())
    {
        LogError("AssetAPI::UploadAssetFromFileInMemory failed! The storage is not writable.");
        return AssetUploadTransferPtr();
    }

    AssetProviderPtr provider = destination->provider.Lock();
    if (!provider)
    {
        LogError("AssetAPI::UploadAssetFromFileInMemory failed! The provider pointer of the passed destination asset storage was null!");
        return AssetUploadTransferPtr();
    }

    AssetUploadTransferPtr transfer = provider->UploadAssetFromFileInMemory(data, numBytes, destination, assetName);
    if (transfer)
        currentUploadTransfers[transfer->destinationStorage.Lock()->GetFullAssetURL(assetName)] = transfer;

    return transfer;
}

void AssetAPI::ForgetAllAssets()
{
    readyTransfers.Clear();
    readySubTransfers.Clear();

    // ForgetBundle removes the bundle it is given to from the assetBundles map, so this loop terminates.
    // All bundle sub assets are unloaded from the assets map below.
    while(!assetBundles.empty())
        ForgetBundle(assetBundles.begin()->second, false);
    assetBundles.clear();
    bundleMonitors.clear();

    // ForgetAsset removes the asset it is given to from the assets map, so this loop terminates.
    while(!assets.empty())
        ForgetAsset(assets.begin()->second, false);
    assets.clear();
   
    // Abort all current transfers.
    while(!currentTransfers.empty())
    {
        AssetTransferPtr abortTransfer = currentTransfers.begin()->second;
        if (!abortTransfer.Get())
        {
            currentTransfers.erase(currentTransfers.begin());
            continue;
        }
        String abortRef = abortTransfer->source.ref;
        abortTransfer->Abort();
        abortTransfer.Reset();
        
        // Make sure the abort chain removed the transfer, otherwise we are in a infinite loop.
        AssetTransferMap::iterator iter = currentTransfers.find(abortRef);
        if (iter != currentTransfers.end())
            currentTransfers.erase(iter);
    }
    currentTransfers.clear();
}

void AssetAPI::Reset()
{
    ForgetAllAssets();
    /// \todo Implement DiskSourceChangeWatcher & AssetCache
    // SAFE_DELETE(assetCache);
    // SAFE_DELETE(diskSourceChangeWatcher);
    assets.clear();
    assetBundles.clear();
    bundleMonitors.clear();
    pendingDownloadRequests.clear();
    assetTypeFactories.Clear();
    assetBundleTypeFactories.Clear();
    defaultStorage.Reset();
    readyTransfers.Clear();
    readySubTransfers.Clear();
    assetDependencies.Clear();
    currentUploadTransfers.clear();
    currentTransfers.clear();
    providers.Clear();
}

Vector<AssetTransferPtr> AssetAPI::PendingTransfers() const
{
    Vector<AssetTransferPtr> transfers;
    for(AssetTransferMap::const_iterator iter = currentTransfers.begin(); iter != currentTransfers.end(); ++iter)
        transfers.Push(iter->second);

    transfers.Push(readyTransfers);
    return transfers;
}

AssetTransferPtr AssetAPI::PendingTransfer(String assetRef) const
{
    AssetTransferMap::const_iterator iter = currentTransfers.find(assetRef);
    if (iter != currentTransfers.end())
        return iter->second;
    for(uint i = 0; i < readyTransfers.Size(); ++i)
        if (readyTransfers[i]->source.ref == assetRef)
            return readyTransfers[i];

    return AssetTransferPtr();
}

AssetTransferPtr AssetAPI::RequestAsset(String assetRef, String assetType, bool forceTransfer)
{
    // This is a function that handles all asset requests made to the Tundra asset system.
    // Note that touching this function has many implications all around the complex asset load routines
    // and its multiple steps. It is quite easy to introduce bugs and break subtle things by modifying this function.
    // * You can request both direct asset references <ref> or asset references into bundles <ref>#subref.
    // * This single function handles requests to not yet loaded assets and already loaded assets.
    //   Here we also handle forced reloads from the source via the forceTransfer parameter if
    //   the asset is already loaded to the system. This function also handles reloading existing
    //   but at the moment unloaded assets.

    PROFILE(AssetAPI_RequestAsset);

    // Turn named storage and default storage specifiers to absolute specifiers.
    assetRef = ResolveAssetRef("", assetRef);
    assetType = assetType.Trimmed();
    if (assetRef.Empty())
        return AssetTransferPtr();

    // Parse out full reference, main asset ref and sub asset ref.
    String fullAssetRef, subAssetPart, mainAssetPart;
    ParseAssetRef(assetRef, 0, 0, 0, 0, 0, 0, 0, &subAssetPart, &fullAssetRef, &mainAssetPart);
    
    // Detect if the requested asset is a sub asset. Replace the lookup ref with the parent bundle reference.
    // Note that bundle handling has its own code paths as we need to load the bundle first before
    // querying the sub asset(s) from it. Bundles cannot be implemented via IAsset, or could but
    // the required hacks would be much worse than a bit more internal AssetAPI code.
    //   1) If we have the sub asset already loaded we return a virtual transfer for it as normal.
    //   2) If we don't have the sub asset but we have the bundle we request the data from the bundle.
    //   3) If we don't have the bundle we request it. If that succeeds we go to 2), if not we fail the sub asset transfer.
    /// @todo Live reloading on disk source changes is not implemented for bundles. Implement if there seems to be need for it. 
    const bool isSubAsset = !subAssetPart.Empty();
    if (isSubAsset) 
        assetRef = mainAssetPart;

    // To optimize, we first check if there is an outstanding request to the given asset. If so, we return that request. In effect, we never
    // have multiple transfers running to the same asset. Important: This must occur before checking the assets map for whether we already have the asset in memory, since
    // an asset will be stored in the AssetMap when it has been downloaded, but it might not yet have all its dependencies loaded.
    AssetTransferMap::iterator ongoingTransferIter = currentTransfers.find(assetRef);
    if (ongoingTransferIter != currentTransfers.end())
    {
        AssetTransferPtr transfer = ongoingTransferIter->second;
        if (forceTransfer && dynamic_cast<VirtualAssetTransfer*>(transfer.Get()))
        {
            // If forceTransfer is on, but the transfer is virtual, log error. This case can not be currently handled properly.
            LogError("AssetAPI::RequestAsset: Received forceTransfer for asset " + assetRef + " while a virtual transfer is already going on");
            return transfer;
        }

        // If this is a sub asset ref to a bundle, we just found the bundle transfer with assetRef. 
        // We need to add this sub asset transfer to the bundles monitor. We return the virtual transfer that
        // will get loaded once the bundle is loaded.
        AssetBundleMonitorMap::iterator bundleMonitorIter = bundleMonitors.find(assetRef);
        if (bundleMonitorIter != bundleMonitors.end())
        {
            AssetBundleMonitorPtr assetBundleMonitor = (*bundleMonitorIter).second;
            if (assetBundleMonitor)
            {
                AssetTransferPtr subTransfer = assetBundleMonitor->SubAssetTransfer(fullAssetRef);
                if (!subTransfer.Get())
                {
                    subTransfer = new VirtualAssetTransfer();
                    subTransfer->source.ref = fullAssetRef;
                    subTransfer->assetType = ResourceTypeForAssetRef(subTransfer->source.ref);
                    subTransfer->provider = transfer->provider;
                    subTransfer->storage = transfer->storage;

                    assetBundleMonitor->AddSubAssetTransfer(subTransfer);
                }
                return subTransfer;
            }
        }

        if (assetType.Empty())
            assetType = ResourceTypeForAssetRef(mainAssetPart);
        if (!assetType.Empty() && !transfer->assetType.Empty() && assetType != transfer->assetType)
        {
            // Check that the requested types were the same. Don't know what to do if they differ, so only print a warning if so.
            LogWarning("AssetAPI::RequestAsset: Asset \"" + assetRef + "\" first requested by type " + 
                transfer->assetType + ", but now requested by type " + assetType + ".");
        }
        
        return transfer;
    }

    // Check if we've already downloaded this asset before and it already is loaded in the system. We never reload an asset we've downloaded before, 
    // unless the client explicitly forces so, or if we get a change notification signal from the source asset provider telling the asset was changed.
    // Note that we are using fullRef here as it has the complete sub asset ref also in it. If this is a sub asset request the assetRef has already been modified.
    AssetPtr existingAsset;
    AssetMap::iterator existingAssetIter = assets.find(fullAssetRef);
    if (existingAssetIter != assets.end())
    {
        existingAsset = existingAssetIter->second;
        if (!assetType.Empty() && assetType != existingAsset->Type())
            LogWarning("AssetAPI::RequestAsset: Tried to request asset \"" + assetRef + "\" by type \"" + assetType + "\". Asset by that name exists, but it is of type \"" + existingAsset->Type() + "\"!");
        assetType = existingAsset->Type();
    }
    else
    {
        // If this is a sub asset we must set the type from the parent bundle.
        if (assetType.Empty())
            assetType = ResourceTypeForAssetRef(mainAssetPart);

        // Null factories are used for not loading particular assets.
        // Having this option we can return a null transfer here to not
        // get lots of error logging eg. on a headless Tundra for UI assets.
        if (dynamic_cast<NullAssetFactory*>(AssetTypeFactory(assetType).Get()))
            return AssetTransferPtr();
    }
    
    // Whenever the client requests an asset that was loaded before, we create a request for that asset nevertheless.
    // The idea is to have the code path run the same independent of whether the asset existed or had to be downloaded, i.e.
    // a request is always made, and the receiver writes only a single asynchronous code path for handling the asset.
    /// @todo Evaluate whether existing->IsLoaded() should rather be existing->IsEmpty().
    if (existingAsset && existingAsset->IsLoaded() && !forceTransfer)
    {
        // The asset was already downloaded. Generate a 'virtual asset transfer' 
        // and return it to the client. Fill in the valid existing asset ptr to the transfer.
        AssetTransferPtr transfer(new VirtualAssetTransfer());
        transfer->asset = existingAsset;
        transfer->source.ref = assetRef;
        transfer->assetType = assetType;
        transfer->provider = transfer->asset->AssetProvider();
        transfer->storage = transfer->asset->AssetStorage();
        transfer->diskSourceType = transfer->asset->DiskSourceType();
        
        // There is no asset provider processing this 'transfer' that would "push" the AssetTransferCompleted call. 
        // We have to remember to do it ourselves via readyTransfers list in Update().
        readyTransfers.Push(transfer); 
        return transfer;
    }    

    // If this is a sub asset request check if its parent bundle is already available.
    // If it is load/reload the asset data to the transfer and use the readyTransfers
    // list to do the AssetTransferCompleted callback on the next frame.
    if (isSubAsset)
    {
        // Check if sub asset transfer is ongoing, meaning we already have been below and its still being processed.
        AssetTransferMap::iterator ongoingSubAssetTransferIter = currentTransfers.find(fullAssetRef);
        if (ongoingSubAssetTransferIter != currentTransfers.end())
            return ongoingSubAssetTransferIter->second;
        
        // Create a new transfer and load the asset from the bundle to it
        AssetBundleMap::iterator bundleIter = assetBundles.find(assetRef);
        if (bundleIter != assetBundles.end())
        {
            // Return existing loader transfer
            for(uint i = 0; i < readySubTransfers.Size(); ++i)
            {
                AssetTransferPtr subTransfer = readySubTransfers[i].subAssetTransfer;
                if (subTransfer.Get() && subTransfer->source.ref.Compare(fullAssetRef, false) == 0)
                    return subTransfer;
            }
            // Create a loader for this sub asset.
            AssetTransferPtr transfer(new VirtualAssetTransfer());
            transfer->asset = existingAsset;
            transfer->source.ref = fullAssetRef;
            readySubTransfers.Push(SubAssetLoader(assetRef, transfer));
            return transfer;
        }
        else
        {
            // For a sub asset the parent bundle needs to 
            // be requested by its proper asset type.
            assetType = ResourceTypeForAssetRef(mainAssetPart);
        }
    }

    // See if there is an asset upload that should block this download. If the same asset is being
    // uploaded and downloaded simultaneously, make the download wait until the upload completes.
    if (currentUploadTransfers.find(assetRef) != currentUploadTransfers.end())
    {
        LogDebug("The download of asset \"" + assetRef + "\" needs to wait, since the same asset is being uploaded at the moment.");
        PendingDownloadRequest pendingRequest;
        pendingRequest.assetRef = assetRef;
        pendingRequest.assetType = assetType;
        pendingRequest.transfer = new IAssetTransfer();

        pendingDownloadRequests[assetRef] = pendingRequest;

        /// @bug Problem. When we return this structure, the client will connect to this.
        return pendingRequest.transfer; 
    }

    // Find the asset provider that will fulfill this request.
    AssetProviderPtr provider = ProviderForAssetRef(assetRef, assetType);
    if (!provider)
    {
        LogError("AssetAPI::RequestAsset: Failed to find a provider for asset \"" + assetRef + "\", type: \"" + assetType + "\"");
        return AssetTransferPtr();
    }

    // Perform the actual request from the provider.
    AssetTransferPtr transfer = provider->RequestAsset(assetRef, assetType);
    if (!transfer)
    {
        LogError("AssetAPI::RequestAsset: Failed to request asset \"" + assetRef + "\", type: \"" + assetType + "\"");
        return AssetTransferPtr();
    }

    // Note that existingAsset can be a valid loaded asset at this point. In this case
    // we can only be this far in this function if forceTransfer is true. This makes the 
    // upcoming download and load process will reload the asset data instead of creating a new asset.
    transfer->asset = existingAsset;
    transfer->provider = provider;

    // Store the newly allocated AssetTransfer internally, so that any duplicated requests to this asset 
    // will return the same request pointer, so we'll avoid multiple downloads to the exact same asset.
    currentTransfers[assetRef] = transfer;

    // Request for a direct asset reference.
    if (!isSubAsset)
        return transfer;
    // Request for a sub asset in a bundle. 
    else
    {       
        // AssetBundleMonitor is for tracking object for this bundle and all pending sub asset transfers.
        // It will connect to the asset transfer and create the bundle on download succeeded or handle it failing
        // by notifying all the child transfer failed.
        AssetBundleMonitorPtr bundleMonitor;
        AssetBundleMonitorMap::iterator bundleMonitorIter = bundleMonitors.find(assetRef);
        if (bundleMonitorIter != bundleMonitors.end())
        {
            // Add the sub asset to an existing bundle monitor.
            bundleMonitor = (*bundleMonitorIter).second;
        }
        else
        {
            // Create new bundle monitor.
            bundleMonitor = new AssetBundleMonitor(this, transfer);
            bundleMonitors[assetRef] = bundleMonitor;
        }
        if (!bundleMonitor.Get())
        {
            LogError("AssetAPI::RequestAsset: Failed to get or create asset bundle: " + assetRef);
            return AssetTransferPtr();
        }
        
        AssetTransferPtr subTransfer = bundleMonitor->SubAssetTransfer(fullAssetRef);
        if (!subTransfer.Get())
        {
            // In this case we return a virtual transfer for the sub asset.
            // This transfer will be loaded once the bundle can provide the content.
            subTransfer = new VirtualAssetTransfer();
            subTransfer->source.ref = fullAssetRef;
            subTransfer->assetType = ResourceTypeForAssetRef(subTransfer->source.ref);
            subTransfer->provider = transfer->provider;
            subTransfer->storage = transfer->storage;
            
            bundleMonitor->AddSubAssetTransfer(subTransfer);
        }
        return subTransfer;
    }
}

AssetTransferPtr AssetAPI::RequestAsset(const AssetReference &ref, bool forceTransfer)
{
    return RequestAsset(ref.ref, ref.type, forceTransfer);
}

AssetProviderPtr AssetAPI::ProviderForAssetRef(String assetRef, String assetType) const
{
    PROFILE(AssetAPI_GetProviderForAssetRef);

    assetType = assetType.Trimmed();
    assetRef = assetRef.Trimmed();

    if (assetType.Length() == 0)
        assetType = ResourceTypeForAssetRef(assetRef.ToLower());

    // If the assetRef is by local filename without a reference to a provider or storage, use the default asset storage in the system for this assetRef.
    String namedStorage;
    AssetRefType assetRefType = ParseAssetRef(assetRef, 0, &namedStorage);
    if (assetRefType == AssetRefRelativePath)
    {
        AssetStoragePtr defaultStorage = DefaultAssetStorage();
        AssetProviderPtr defaultProvider = (defaultStorage ? defaultStorage->provider.Lock() : AssetProviderPtr());
        if (defaultProvider)
            return defaultProvider;
        // If no default provider, intentionally fall through to lookup each asset provider in turn.
    }
    else if (assetRefType == AssetRefNamedStorage) // The asset ref explicitly points to a named storage. Use the provider for that storage.
    {
        AssetStoragePtr storage = AssetStorageByName(namedStorage);
        AssetProviderPtr provider = (storage ? storage->provider.Lock() : AssetProviderPtr());
        return provider;
    }

    Vector<AssetProviderPtr> providers = AssetProviders();
    for(uint i = 0; i < providers.Size(); ++i)
        if (providers[i]->IsValidRef(assetRef, assetType))
            return providers[i];

    return AssetProviderPtr();
}

String AssetAPI::ResolveAssetRef(String context, String assetRef) const
{
    if (assetRef.Trimmed().Empty())
        return "";

    context = context.Trimmed();

    // First see if we have an exact match for the ref to an existing asset.
    AssetMap::const_iterator iter = assets.find(assetRef);
    if (iter != assets.end())
        return assetRef; // Use the ref as-is, there's an existing asset to map this string to.

    // If the assetRef is by local filename without a reference to a provider or storage, use the default asset storage in the system for this assetRef.
    String assetPath;
    String namedStorage;
    String fullRef;
    AssetRefType assetRefType = ParseAssetRef(assetRef, 0, &namedStorage, 0, &assetPath, 0, 0, 0, 0, &fullRef);
    assetRef = fullRef; // The first thing we do is normalize the form of the ref. This means e.g. adding 'http://' in front of refs that look like 'www.server.com/'.
    
    switch(assetRefType)
    {
    case AssetRefLocalPath: // Absolute path like "C:\myassets\texture.png".
    case AssetRefLocalUrl: // Local path using "local://", like "local://file.mesh".
    case AssetRefExternalUrl: // External path using an URL specifier, like "http://server.com/file.mesh" or "someProtocol://server.com/asset.dat".
        return assetRef;
    case AssetRefRelativePath:
        {
            if (context.Empty())
            {
                AssetStoragePtr defaultStorage = DefaultAssetStorage();
                if (!defaultStorage)
                    return assetRef; // Failed to find the provider, just use the ref as it was, and hope. (It might still be satisfied by the local storage).
                return defaultStorage->GetFullAssetURL(assetRef);
            }
            else
            {
                // Join the context to form a full url, e.g. context: "http://myserver.com/path/myasset.material", ref: "texture.png" returns "http://myserver.com/path/texture.png".
                String contextPath;
                String contextNamedStorage;
                String contextProtocolSpecifier;
                String contextSubAssetPart;
                String contextMainPart;
                AssetRefType contextRefType = ParseAssetRef(context, &contextProtocolSpecifier, &contextNamedStorage, 0, 0, 0, &contextPath, 0, &contextSubAssetPart, 0, &contextMainPart);
                if (contextRefType == AssetRefRelativePath || contextRefType == AssetRefInvalid)
                {
                    LogError("Asset ref context \"" + contextPath + "\" is a relative path and cannot be used as a context for lookup for ref \"" + assetRef + "\"!");
                    return assetRef; // Return at least something.
                }
                
                // Context is a asset bundle sub asset
                if (!contextSubAssetPart.Empty())
                {
                    // Context: bundle#subfolder/subasset -> bundle#subfolder/<assetPath>
                    if (contextSubAssetPart.Contains("/"))
                        return contextMainPart + "#" + contextSubAssetPart.Substring(0, contextSubAssetPart.FindLast("/")+1) + assetPath;
                    // Context: bundle#subasset -> bundle#<assetPath>
                    else
                        return contextMainPart + "#" + assetPath;
                }
                
                String newAssetRef;
                if (!contextNamedStorage.Empty())
                    newAssetRef = contextNamedStorage + ":";
                else if (!contextProtocolSpecifier.Empty())
                    newAssetRef = contextProtocolSpecifier + "://";
                /// \todo Qt cleanPath() function used here
                newAssetRef += contextPath + assetPath;
                return newAssetRef;
            }
        }
        break;
    case AssetRefNamedStorage: // The asset ref explicitly points to a named storage. Use the provider for that storage.
        {
            AssetStoragePtr storage = AssetStorageByName(namedStorage);
            if (!storage)
                return assetRef; // Failed to find the provider, just use the ref as it was, and hope.
            return storage->GetFullAssetURL(assetPath);
        }
        break;
    }
    assert(false);
    return assetRef;
}

void AssetAPI::RegisterAssetTypeFactory(AssetTypeFactoryPtr factory)
{
    AssetTypeFactoryPtr existingFactory = AssetTypeFactory(factory->Type());
    if (existingFactory)
    {
        LogWarning("AssetAPI::RegisterAssetTypeFactory: Factory with type '" + factory->Type() + "' already registered.");
        return;
    }

    assert(factory->Type() == factory->Type().Trimmed());
    assetTypeFactories.Push(factory);
}

void AssetAPI::RegisterAssetBundleTypeFactory(AssetBundleTypeFactoryPtr factory)
{
    AssetBundleTypeFactoryPtr existingFactory = AssetBundleTypeFactory(factory->Type());
    if (existingFactory)
    {
        LogWarning("AssetAPI::RegisterAssetBundleTypeFactory: Factory with type '" + factory->Type() + "' already registered.");
        return;
    }

    assert(factory->Type() == factory->Type().Trimmed());
    assetBundleTypeFactories.Push(factory);
}

String AssetAPI::GenerateUniqueAssetName(String assetTypePrefix, String assetNamePrefix) const
{
    static unsigned long uniqueRunningAssetCounter = 1;

    assetTypePrefix = assetTypePrefix.Trimmed();
    assetNamePrefix = assetNamePrefix.Trimmed();

    if (assetTypePrefix.Empty())
        assetTypePrefix = "Asset";

    String assetName;
    // We loop until we manage to generate a single asset name that does not exist, incrementing a running counter at each iteration.
    for(int i = 0; i < 10000; ++i) // The intent is to loop 'infinitely' until a name is found, but do an artificial limit to avoid voodoo bugs.
    {
        assetName = assetTypePrefix + "_" + assetNamePrefix + (assetNamePrefix.Empty() ? "" : "_") + String(uniqueRunningAssetCounter++);
        if (!FindAsset(assetName))
            return assetName;
    }
    LogError("GenerateUniqueAssetName failed!");
    return String();
}

String AssetAPI::GenerateTemporaryNonexistingAssetFilename(String filenameSuffix) const
{
    Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();

    // Create this file path into the cache dir to avoid
    // windows non-admin users having no write permission to the run folder
    String cacheDir;
    /// \todo Implement AssetCache
    //if (assetCache)
    //    cacheDir = assetCache->CacheDirectory());
    //else
        cacheDir = fw->UserDataDirectory();

    if (fileSystem->DirExists(cacheDir))
    {
        static unsigned long uniqueRunningFilenameCounter = 1;
        String filename;
        // We loop until we manage to generate a single filename that does not exist, incrementing a running counter at each iteration.
        for(int i = 0; i < 10000; ++i) // The intent is to loop 'infinitely' until a name is found, but do an artificial limit to avoid voodoo bugs.
        {
            filename = GuaranteeTrailingSlash(cacheDir) + "temporary_" + String(uniqueRunningFilenameCounter++) + "_" + SanitateAssetRef(filenameSuffix.Trimmed());
            if (!fileSystem->FileExists(filename))
                return filename;
        }
    }
    LogError("GenerateTemporaryNonexistingAssetFilename failed!");
    return String();
}

AssetPtr AssetAPI::CreateNewAsset(String type, String name)
{
    return CreateNewAsset(type, name, AssetStoragePtr());
}

AssetPtr AssetAPI::CreateNewAsset(String type, String name, AssetStoragePtr storage)
{
    PROFILE(AssetAPI_CreateNewAsset);
    type = type.Trimmed();
    name = name.Trimmed();
    if (name.Length() == 0)
    {
        LogError("AssetAPI:CreateNewAsset: Trying to create an asset with name=\"\"!");
        return AssetPtr();
    }

    AssetTypeFactoryPtr factory = AssetTypeFactory(type);
    if (!factory)
    {
        // This spams too much with the server giving us storages, debug should be fine for things we dont have a factory.
        LogDebug("AssetAPI:CreateNewAsset: Cannot create asset of type \"" + type + "\", name: \"" + name + "\". No type factory registered for the type!");
        return AssetPtr();
    }
    if (dynamic_cast<NullAssetFactory*>(factory.Get()))
        return AssetPtr();
    AssetPtr asset = factory->CreateEmptyAsset(this, name);

    if (!asset)
    {
        LogError("AssetAPI:CreateNewAsset: IAssetTypeFactory::CreateEmptyAsset(type \"" + type + "\", name: \"" + name + "\") failed to create asset!");
        return AssetPtr();
    }
    assert(asset->IsEmpty());

    // Fill the provider & storage for the new asset already here if possible
    ///\todo Revisit the logic below, and the callers of CreateNewAsset. Is it possible to remove the following
    /// calls, and have the caller directly set the storage?
    if (!storage)
    {
        asset->SetAssetProvider(ProviderForAssetRef(type, name));
        asset->SetAssetStorage(StorageForAssetRef(name));
    }
    else
    {
        asset->SetAssetProvider(storage->provider.Lock());
        asset->SetAssetStorage(storage);
    }

    // Remember this asset in the global AssetAPI storage.
    assets[name] = asset;

    ///\bug DiskSource and DiskSourceType are not set yet.
    {
        PROFILE(AssetAPI_CreateNewAsset_emit_AssetCreated);
        AssetCreated.Emit(asset);
    }
    
    return asset;
}

AssetBundlePtr AssetAPI::CreateNewAssetBundle(String type, String name)
{
    PROFILE(AssetAPI_CreateNewAssetBundle);
    type = type.Trimmed();
    name = name.Trimmed();
    if (name.Length() == 0)
    {
        LogError("AssetAPI:CreateNewAssetBundle: Trying to create an asset with name=\"\"!");
        return AssetBundlePtr();
    }

    AssetBundleTypeFactoryPtr factory = AssetBundleTypeFactory(type);
    if (!factory)
    {
        // This spams too much with the server giving us storages, debug should be fine for things we don't have a factory.
        LogError("AssetAPI:CreateNewAssetBundle: Cannot create asset of type \"" + type + "\", name: \"" + name + "\". No type factory registered for the type!");
        return AssetBundlePtr();
    }
    AssetBundlePtr assetBundle = factory->CreateEmptyAssetBundle(this, name);
    if (!assetBundle)
    {
        LogError("AssetAPI:CreateNewAssetBundle: IAssetTypeFactory::CreateEmptyAsset(type \"" + type + "\", name: \"" + name + "\") failed to create asset!");
        return AssetBundlePtr();
    }

    assetBundle->SetAssetProvider(ProviderForAssetRef(type, name));
    assetBundle->SetAssetStorage(StorageForAssetRef(name));

    // Remember this asset bundle in the global AssetAPI storage.
    assetBundles[name] = assetBundle;
    return assetBundle;
}

bool AssetAPI::LoadSubAssetToTransfer(AssetTransferPtr transfer, const String &bundleRef, const String &fullSubAssetRef, String subAssetType)
{
    if (!transfer)
    {
        LogError("LoadSubAssetToTransfer: Transfer is null, cannot continue to load '" + fullSubAssetRef + "'.");
        return false;
    }

    AssetBundleMap::iterator bundleIter = assetBundles.find(bundleRef);
    if (bundleIter != assetBundles.end())
        return LoadSubAssetToTransfer(transfer, (*bundleIter).second.Get(), fullSubAssetRef, subAssetType);
    else
    {
        LogError("LoadSubAssetToTransfer: Asset bundle '" + bundleRef + "' not loaded, cannot continue to load '" + fullSubAssetRef + "'.");
        return false;
    }
}

bool AssetAPI::LoadSubAssetToTransfer(AssetTransferPtr transfer, IAssetBundle *bundle, const String &fullSubAssetRef, String subAssetType)
{
    if (!transfer)
    {
        LogError("LoadSubAssetToTransfer: Transfer is null, cannot continue to load '" + fullSubAssetRef + "'.");
        return false;
    }
    if (!bundle)
    {
        LogError("LoadSubAssetToTransfer: Source AssetBundle is null, cannot continue to load '" + fullSubAssetRef + "'.");
        return false;
    }

    String subAssetRef;
    if (subAssetType.Empty())
        subAssetType = ResourceTypeForAssetRef(fullSubAssetRef);
    ParseAssetRef(fullSubAssetRef, 0, 0, 0, 0, 0, 0, 0, &subAssetRef);
    
    transfer->source.ref = fullSubAssetRef;
    transfer->source.type = subAssetType;
    transfer->assetType = subAssetType;

    // Avoid data shuffling if disk source is valid. IAsset loading can 
    // manage with one of these, it does not need them both.
    Vector<u8> subAssetData;
    String subAssetDiskSource = bundle->GetSubAssetDiskSource(subAssetRef);
    if (subAssetDiskSource.Empty()) 
    {
        subAssetData = bundle->GetSubAssetData(subAssetRef);
        if (subAssetData.Size() == 0)
        {
            String error("AssetAPI: Failed to load sub asset '" + fullSubAssetRef + " from bundle '" + bundle->Name() + "': Sub asset does not exist.");
            LogError(error);
            transfer->EmitAssetFailed(error);
            return false;
        }
    }

    if (!transfer->asset)
        transfer->asset = CreateNewAsset(subAssetType, fullSubAssetRef);
    if (!transfer->asset)
    {
        String error("AssetAPI: Failed to create new sub asset of type \"" + subAssetType + "\" and name \"" + fullSubAssetRef + "\"");
        LogError(error);
        transfer->EmitAssetFailed(error);
        return false;
    }
    
    transfer->asset->SetDiskSource(subAssetDiskSource);
    transfer->asset->SetDiskSourceType(IAsset::Bundle);
    transfer->asset->SetAssetStorage(transfer->storage.Lock());
    transfer->asset->SetAssetProvider(transfer->provider.Lock());

    // Add the sub asset transfer to the current transfers map now.
    // This will ensure that the rest of the loading procedure will continue
    // like it normally does in AssetAPI and the requesting parties don't have to
    // know about how asset bundles are packed or request them before the sub asset in any way.
    currentTransfers[fullSubAssetRef] = transfer;

    // Connect to Loaded() signal of the asset to be able to notify any dependent assets
    transfer->asset.Get()->Loaded.Connect(this, &AssetAPI::OnAssetLoaded);

    // Tell everyone this transfer has now been downloaded. Note that when this signal is fired, the asset dependencies may not yet be loaded.
    transfer->EmitAssetDownloaded();

    bool success = false;
    if (subAssetData.Size() > 0)
        success = transfer->asset->LoadFromFileInMemory(&subAssetData[0], subAssetData.Size());
    else if (!transfer->asset->DiskSource().Empty())
        success = transfer->asset->LoadFromFile(subAssetDiskSource);

    // If the load from either of in memory data or file data failed, update the internal state.
    // Otherwise the transfer will be left dangling in currentTransfers. For successful loads
    // we do no need to call AssetLoadCompleted because success can mean asynchronous loading,
    // in which case the call will arrive once the asynchronous loading is completed.
    if (!success)
        AssetLoadFailed(fullSubAssetRef);
    return success;
}

AssetTypeFactoryPtr AssetAPI::AssetTypeFactory(const String &typeName) const
{
    PROFILE(AssetAPI_AssetTypeFactory);
    for(uint i = 0; i < assetTypeFactories.Size(); ++i)
        if (assetTypeFactories[i]->Type().Compare(typeName, false) == 0)
            return assetTypeFactories[i];

    return AssetTypeFactoryPtr();
}

AssetBundleTypeFactoryPtr AssetAPI::AssetBundleTypeFactory(const String &typeName) const
{
    PROFILE(AssetAPI_AssetBundleTypeFactory);
    for(uint i = 0; i < assetBundleTypeFactories.Size(); ++i)
        if (assetBundleTypeFactories[i]->Type().Compare(typeName, false) == 0)
            return assetBundleTypeFactories[i];

    return AssetBundleTypeFactoryPtr();
}

AssetPtr AssetAPI::FindAsset(String assetRef) const
{
    // First try to see if the ref has an exact match.
    AssetMap::const_iterator iter = assets.find(assetRef);
    if (iter != assets.end())
        return iter->second;

    // If not, normalize and resolve the lookup of the given asset.
    assetRef = ResolveAssetRef("", assetRef);

    iter = assets.find(assetRef);
    if (iter != assets.end())
        return iter->second;
    return AssetPtr();
}

AssetBundlePtr AssetAPI::FindBundle(String bundleRef) const
{
    // First try to see if the ref has an exact match.
    AssetBundleMap::const_iterator iter = assetBundles.find(bundleRef);
    if (iter != assetBundles.end())
        return iter->second;

    // If not, normalize and resolve the lookup of the given asset bundle.
    bundleRef = ResolveAssetRef("", bundleRef);

    iter = assetBundles.find(bundleRef);
    if (iter != assetBundles.end())
        return iter->second;
    return AssetBundlePtr();
}

void AssetAPI::Update(float frametime)
{
    PROFILE(AssetAPI_Update);

    for(uint i = 0; i < providers.Size(); ++i)
        providers[i]->Update(frametime);

    // Proceed with ready transfers.
    if (readyTransfers.Size() > 0)
    {
        // Normally it is the AssetProvider's responsibility to call AssetTransferCompleted when a download finishes.
        // The 'readyTransfers' list contains all the asset transfers that don't have any AssetProvider serving them. These occur in two cases:
        // 1) A client requested an asset that was already loaded. In that case the request is not given to any assetprovider, but delayed in readyTransfers
        //    for one frame after which we just signal the asset to have been loaded.
        // 2) We found the asset from disk cache. No need to ask an assetprovider

        // Call AssetTransferCompleted manually for any asset that doesn't have an AssetProvider serving it. ("virtual transfers").
        for(uint i = 0; i < readyTransfers.Size(); ++i)
            AssetTransferCompleted(readyTransfers[i].Get());
        readyTransfers.Clear();
    }
    
    // Proceed with ready sub asset transfers.
    if (readySubTransfers.Size() > 0)
    {
        // readySubTransfers contains sub asset transfers to loaded bundles. The sub asset loading cannot be completed in RequestAsset
        // as it would trigger signals before the calling code can receive and hook to the AssetTransfer. We delay calling LoadSubAssetToTransfer
        // into this function so that all is hooked and loading can be done normally. This is very similar to the above case for readyTransfers.
        for(uint i = 0; i < readySubTransfers.Size(); ++i)
        {
            AssetTransferPtr subTransfer = readySubTransfers[i].subAssetTransfer;
            LoadSubAssetToTransfer(subTransfer, readySubTransfers[i].parentBundleRef, subTransfer->source.ref);
            subTransfer.Reset();
        }
        readySubTransfers.Clear();
    }
}

String GuaranteeTrailingSlash(const String &source)
{
    String s = source.Trimmed();
    if (s.Empty())
        return ""; // If user inputted "", output "" (can't output "/", since that would mean the root of the whole filesystem on linux)

    if (s[s.Length()-1] != '/' && s[s.Length()-1] != '\\')
        s = s + "/";

    return s;
}

HashMap<String, String> ParseAssetRefArgs(const String &url, String *body)
{
    HashMap<String, String> keyValues;

    uint paramsStartIndex = url.Find("?");
    if (paramsStartIndex == String::NPOS)
    {
        if (body)
            *body = url;
        return keyValues;
    }
    String args = url.Substring(paramsStartIndex+1);
    if (body)
        *body = url.Substring(0, paramsStartIndex);
    StringVector params = args.Split('&');
    foreach(String param, params)
    {
        StringVector keyValue = param.Split('=');
        if (keyValue.Size() == 0 || keyValue.Size() > 2)
            continue; // Silently ignore malformed data.
        String key = keyValue[0].Trimmed();
        String value = keyValue[1].Trimmed();

        keyValues[key] = value;
    }
    return keyValues;
}

AssetTransferMap::iterator AssetAPI::FindTransferIterator(String assetRef)
{
    return currentTransfers.find(assetRef);
}

AssetTransferMap::const_iterator AssetAPI::FindTransferIterator(String assetRef) const
{
    return currentTransfers.find(assetRef);
}

AssetTransferMap::iterator AssetAPI::FindTransferIterator(IAssetTransfer *transfer)
{
    if (!transfer)
        return currentTransfers.end();

    for(AssetTransferMap::iterator iter = currentTransfers.begin(); iter != currentTransfers.end(); ++iter)
        if (iter->second.Get() == transfer)
            return iter;

    return currentTransfers.end();
}

AssetTransferMap::const_iterator AssetAPI::FindTransferIterator(IAssetTransfer *transfer) const
{
    if (!transfer)
        return currentTransfers.end();

    for(AssetTransferMap::const_iterator iter = currentTransfers.begin(); iter != currentTransfers.end(); ++iter)
        if (iter->second.Get() == transfer)
            return iter;

    return currentTransfers.end();
}

void AssetAPI::AssetTransferCompleted(IAssetTransfer *transfer_)
{
    PROFILE(AssetAPI_AssetTransferCompleted);
    
    assert(transfer_);
    
    // At this point, the transfer can originate from several different things:
    // 1) It could be a real AssetTransfer from a real AssetProvider.
    // 2) It could be an AssetTransfer to an Asset that was already downloaded before, in which case transfer_->asset is already filled and loaded at this point.
    // 3) It could be an AssetTransfer that was fulfilled from the disk cache, in which case no AssetProvider was invoked to get here. (we used the readyTransfers queue for this).
        
    AssetTransferPtr transfer(transfer_); // Elevate to a SharedPtr immediately to keep at least one ref alive of this transfer for the duration of this function call.
    //LogDebug("Transfer of asset \"" + transfer->assetType + "\", name \"" + transfer->source.ref + "\" succeeded.");

    // This is a duplicated transfer to an asset that has already been previously loaded. Only signal that the asset's been loaded and finish.
    if (dynamic_cast<VirtualAssetTransfer*>(transfer_) && transfer->asset && transfer->asset->IsLoaded()) 
    {
        transfer->EmitAssetDownloaded();
        transfer->EmitTransferSucceeded();
        pendingDownloadRequests.erase(transfer->source.ref);
        AssetTransferMap::iterator iter = FindTransferIterator(transfer.Get());
        if (iter != currentTransfers.end())
            currentTransfers.erase(iter);
        return;
    }

    // We should be tracking this transfer in an internal data structure.
    AssetTransferMap::iterator iter = FindTransferIterator(transfer_);
    if (iter == currentTransfers.end())
        LogError("AssetAPI: Asset \"" + transfer->assetType + "\", name \"" + transfer->source.ref + "\" transfer finished, but no corresponding AssetTransferPtr was tracked by AssetAPI!");

    // Transfer is for an asset bundle.
    AssetBundleMonitorMap::iterator bundleIter = bundleMonitors.find(transfer->source.ref);
    if (bundleIter != bundleMonitors.end())
    {
        AssetBundlePtr assetBundle = CreateNewAssetBundle(transfer->assetType, transfer->source.ref);
        if (assetBundle)
        {
            // Hook to the success and fail signals. They can either be emitted when we load the asset below or after asynch loading.
            assetBundle->Loaded.Connect(this, &AssetAPI::AssetBundleLoadCompleted);
            assetBundle->Failed.Connect(this, &AssetAPI::AssetBundleLoadFailed);

            // Cache the bundle.
            String bundleDiskSource = transfer->DiskSource(); // The asset provider may have specified an explicit filename to use as a disk source.
            /// \todo Implement AssetCache
            //if (transfer->CachingAllowed() && transfer->rawAssetData.Size() > 0 && assetCache)
            //    bundleDiskSource = assetCache->StoreAsset(&transfer->rawAssetData[0], transfer->rawAssetData.Size(), transfer->source.ref);
            assetBundle->SetDiskSource(bundleDiskSource);

            // The bundle has now been downloaded and cached (if allowed by policy).
            transfer->EmitAssetDownloaded();

            // Load the bundle assets from the transfer data or cache.
            // 1) Try to load from above disk source.
            // 2) If disk source deserialization fails, try in memory loading if bundle allows it with RequiresDiskSource.
            // Note: Be careful not to use bundleIter after DeserializeFromDiskSource is called, as it can righ away emit
            // Loaded and we end up to AssetBundleLoadCompleted that will remove this bundleIter from bundleMonitors map.
            bool success = assetBundle->DeserializeFromDiskSource();
            if (!success && !assetBundle->RequiresDiskSource())
            {
                const u8 *bundleData = (transfer->rawAssetData.Size() > 0 ? &transfer->rawAssetData[0] : 0);
                if (bundleData)
                    success = assetBundle->DeserializeFromData(bundleData, transfer->rawAssetData.Size());
            }

            // If all of the above returned false, this means asset could not be loaded.
            // Call AssetLoadFailed for the bundle to propagate this information to the waiting sub asset transfers. 
            // Propagating will be done automatically in the AssetBundleMonitor when the bundle fails.
            if (!success)
                AssetLoadFailed(transfer->asset->Name());
        }
        else
        {
            // Note that this is monitored by the AssetBundleMonitor and it fail all sub asset transfers at that point.
            // These child transfers are not in the currentTransfers map so we don't have to do cleanup there.
            String error("AssetAPI: Failed to create new asset bundle of type \"" + transfer->assetType + "\" and name \"" + transfer->source.ref + "\"");
            LogError(error);
            transfer->EmitAssetFailed(error);
            
            // Cleanup
            currentTransfers.erase(iter);
            bundleMonitors.erase(bundleIter);
            return;
        }
    }
    // Transfer is for a normal asset.
    else
    {
        // We've finished an asset data download, now create an actual instance of an asset of that type if it did not exist already
        if (!transfer->asset)
            transfer->asset = CreateNewAsset(transfer->assetType, transfer->source.ref);
        if (!transfer->asset)
        {
            String error("AssetAPI: Failed to create new asset of type \"" + transfer->assetType + "\" and name \"" + transfer->source.ref + "\"");
            LogError(error);
            transfer->EmitAssetFailed(error);
            return;
        }

        // Connect to Loaded() signal of the asset to be able to notify any dependent assets
        transfer->asset->Loaded.Connect(this, &AssetAPI::OnAssetLoaded);

        // Save this asset to cache, and find out which file will represent a cached version of this asset.
        String assetDiskSource = transfer->DiskSource(); // The asset provider may have specified an explicit filename to use as a disk source.
        /// \todo Implement AssetCache
        //if (transfer->CachingAllowed() && transfer->rawAssetData.Size() > 0 && assetCache)
        //    assetDiskSource = assetCache->StoreAsset(&transfer->rawAssetData[0], transfer->rawAssetData.Size(), transfer->source.ref);

        // If disksource is still empty, forcibly look up if the asset exists in the cache now.
        //if (assetDiskSource.Empty() && assetCache)
        //    assetDiskSource = assetCache->FindInCache(transfer->source.ref);
        
        // Save for the asset the storage and provider it came from.
        transfer->asset->SetDiskSource(assetDiskSource.Trimmed());
        transfer->asset->SetDiskSourceType(transfer->diskSourceType);
        transfer->asset->SetAssetStorage(transfer->storage.Lock());
        transfer->asset->SetAssetProvider(transfer->provider.Lock());

        // Tell everyone this transfer has now been downloaded. Note that when this signal is fired, the asset dependencies may not yet be loaded.
        transfer->EmitAssetDownloaded();

        bool success = false;
        const u8 *data = (transfer->rawAssetData.Size() > 0 ? &transfer->rawAssetData[0] : 0);
        if (data)
            success = transfer->asset->LoadFromFileInMemory(data, transfer->rawAssetData.Size());
        else
            success = transfer->asset->LoadFromFile(transfer->asset->DiskSource());

        // If the load from either of in memory data or file data failed, update the internal state.
        // Otherwise the transfer will be left dangling in currentTransfers. For successful loads
        // we do no need to call AssetLoadCompleted because success can mean asynchronous loading,
        // in which case the call will arrive once the asynchronous loading is completed.
        if (!success)
            AssetLoadFailed(transfer->asset->Name());
    }
}

void AssetAPI::AssetTransferFailed(IAssetTransfer *transfer, String reason)
{
    if (!transfer)
        return;
        
    LogError("Transfer of asset \"" + transfer->assetType + "\", name \"" + transfer->source.ref + "\" failed! Reason: \"" + reason + "\"");

    ///\todo In this function, there is a danger of reaching an infinite recursion. Remember recursion parents and avoid infinite loops. (A -> B -> C -> A)

    AssetTransferMap::iterator iter = currentTransfers.find(transfer->source.ref);
    if (iter == currentTransfers.end())
        LogError("AssetAPI: Asset \"" + transfer->assetType + "\", name \"" + transfer->source.ref + "\" transfer failed, but no corresponding AssetTransferPtr was tracked by AssetAPI!");

    // Signal any listeners that this asset transfer failed.
    transfer->EmitAssetFailed(reason);

    // Propagate the failure of this asset transfer to all assets which depend on this asset.
    Vector<AssetPtr> dependents = FindDependents(transfer->source.ref);
    for(uint i = 0; i < dependents.Size(); ++i)
    {
        AssetTransferPtr dependentTransfer = PendingTransfer(dependents[i]->Name());
        if (dependentTransfer)
        {
            String failReason = "Transfer of dependency " + transfer->source.ref + " failed due to reason: \"" + reason + "\"";
            AssetTransferFailed(dependentTransfer.Get(), failReason);
        }
    }

    pendingDownloadRequests.erase(transfer->source.ref);
    if (iter != currentTransfers.end())
        currentTransfers.erase(iter);
}

void AssetAPI::AssetTransferAborted(IAssetTransfer *transfer)
{
    if (!transfer)
        return;
        
    // Don't log any errors for aborted transfers. This is unwanted spam when we disconnect 
    // from a server and have x amount of pending transfers that get aborter.
    AssetTransferMap::iterator iter = currentTransfers.find(transfer->source.ref);
    
    transfer->EmitAssetFailed("Transfer aborted.");   

    // Propagate the failure of this asset transfer to all assets which depend on this asset.
    Vector<AssetPtr> dependents = FindDependents(transfer->source.ref);
    for(uint i = 0; i < dependents.Size(); ++i)
    {
        AssetTransferPtr dependentTransfer = PendingTransfer(dependents[i]->Name());
        if (dependentTransfer)
            AssetTransferAborted(dependentTransfer.Get());
    }

    pendingDownloadRequests.erase(transfer->source.ref);
    if (iter != currentTransfers.end())
        currentTransfers.erase(iter);
}

void AssetAPI::AssetLoadCompleted(const String assetRef)
{
    PROFILE(AssetAPI_AssetLoadCompleted);

    AssetPtr asset;
    AssetTransferMap::const_iterator iter = FindTransferIterator(assetRef);
    AssetMap::iterator iter2 = assets.find(assetRef);
    
    // Check for new transfer: not in the assets map yet
    if (iter != currentTransfers.end())
        asset = iter->second->asset;
    // Check for a reload: is in the known asset map.
    if (!asset.Get() && iter2 != assets.end())
    {
        /** The above transfer might have been found but its IAsset can be null
            in the case that this is a IAsset created by cloning. The code that 
            requested the clone/generated ref might have created the above valid transfer
            but it might have failed, resulting in a null IAsset, if the clone was not made
            before the request. */
        asset = iter2->second;
    }

    if (asset.Get())
    {
        asset->LoadCompleted();

        // Add to watch this path for changed, note this does nothing if the path is already added
        // so we should not be having duplicate paths and/or double emits on changes.
        const String diskSource = asset->DiskSource();

        /// \todo Implement DiskSourceChangeWatcher
        /*
        if (diskSourceChangeWatcher && !diskSource.Empty())
        {
            // If available, check the storage whether assets loaded from it should be live-updated.
            PROFILE(AssetAPI_AssetLoadCompleted_DiskWatcherSetup);
            // By default disable liveupdate for all assets which come outside any known Tundra asset storage.
            // This is because the feature is most likely not used, and setting up the watcher below consumes
            // system resources and CPU time. To enable the disk watcher, make sure that the asset comes from
            // a storage known to Tundra and set liveupdate==true for that asset storage.
            // Note that this means that also local assets outside all storages with absolute path names like 
            // 'C:\mypath\asset.png' will always have liveupdate disabled.
            AssetStoragePtr storage = asset->AssetStorage();
            if (storage)
            {
                bool shouldLiveUpdate = storage->HasLiveUpdate();

                // Localassetprovider implements its own watcher. Therefore only add paths which refer to the assetcache to the AssetAPI watcher
                if (shouldLiveUpdate && (!assetCache || !diskSource.StartsWith(assetCache->CacheDirectory(), false)))
                    shouldLiveUpdate = false;

                if (shouldLiveUpdate)
                {
                    diskSourceChangeWatcher->removePath(diskSource);
                    diskSourceChangeWatcher->addPath(diskSource);
                }
            }
        }
        */

        {
            PROFILE(AssetAPI_AssetLoadCompleted_ProcessDependencies);

            // If this asset depends on any other assets, we have to make asset requests for those assets as well (and all assets that they refer to, and so on).
            RequestAssetDependencies(asset);

            // If we don't have any outstanding dependencies for the transfer, succeed and remove the transfer.
            // Find the iter again as AssetDependenciesCompleted can be called from OnAssetLoaded for synchronous (eg. local://) loads.
            iter = FindTransferIterator(assetRef);
            if (iter != currentTransfers.end() && !HasPendingDependencies(asset))
                AssetDependenciesCompleted(iter->second);
        }
    }
    else
        LogError("AssetAPI: Asset \"" + assetRef + "\" load completed, but no corresponding transfer or existing asset is being tracked!");
}

void AssetAPI::AssetLoadFailed(const String assetRef)
{
    AssetTransferMap::iterator iter = FindTransferIterator(assetRef);
    AssetMap::const_iterator iter2 = assets.find(assetRef);

    if (iter != currentTransfers.end())
    {
        AssetTransferPtr transfer = iter->second;
        transfer->EmitAssetFailed("Failed to load " + transfer->assetType + " '" + transfer->source.ref + "' from asset data.");
        currentTransfers.erase(iter);
    }
    else if (iter2 != assets.end())
        LogError("AssetAPI: Failed to reload asset '" + iter2->second->Name() + "'");
    else
        LogError("AssetAPI: Asset '" + assetRef + "' load failed, but no corresponding transfer or existing asset is being tracked!");
}

void AssetAPI::AssetBundleLoadCompleted(IAssetBundle *bundle)
{
    LogDebug("Asset bundle load completed: " + bundle->Name());
    
    // First erase the transfer as the below sub asset loading can trigger new
    // dependency asset requests to the bundle. In this case we want to load them from the
    // completed asset bundle not add them to the monitors queue.
    AssetTransferMap::iterator bundleTransferIter = FindTransferIterator(bundle->Name());
    if (bundleTransferIter != currentTransfers.end())
        currentTransfers.erase(bundleTransferIter);
    else
        LogWarning("AssetAPI: Asset bundle load completed, but transfer was not tracked: " + bundle->Name());
    
    AssetBundleMonitorMap::iterator monitorIter = bundleMonitors.find(bundle->Name());
    if (monitorIter != bundleMonitors.end())
    {
        // We have no need for the monitor anymore as the AssetBundle has been added to the assetBundles map earlier for reuse.
        AssetBundleMonitorPtr bundleMonitor = (*monitorIter).second;
        Vector<AssetTransferPtr> subTransfers = bundleMonitor->SubAssetTransfers();
        bundleMonitors.erase(monitorIter);
        
        // Start the load process for all sub asset transfers now. From here on out the normal asset request flow should followed.
        for (Vector<AssetTransferPtr>::Iterator subIter = subTransfers.Begin(); subIter != subTransfers.End(); ++subIter)
            LoadSubAssetToTransfer((*subIter), bundle, (*subIter)->source.ref);
    }
    else
        LogWarning("AssetAPI: Asset bundle load completed, but bundle monitor cannot be found: " + bundle->Name());
}

void AssetAPI::AssetBundleLoadFailed(IAssetBundle *bundle)
{
    AssetLoadFailed(bundle->Name());
    
    // We have no need for the monitor anymore as the AssetBundle has been added to the assetBundles map earlier for reuse.
    AssetBundleMonitorMap::iterator monitorIter = bundleMonitors.find(bundle->Name());
    if (monitorIter != bundleMonitors.end())
        bundleMonitors.erase(monitorIter);
}

void AssetAPI::AssetUploadTransferCompleted(IAssetUploadTransfer *uploadTransfer)
{
    String assetRef = uploadTransfer->AssetRef();
    
    // Clear our cache of this data.
    /// @note We could actually update our cache with the same version of the asset that we just uploaded,
    /// to avoid downloading what we just uploaded. That can be implemented later.
    /// \todo Implement AssetCache
    //if (assetCache)
    //    assetCache->DeleteAsset(assetRef);
    
    // If we have the asset (with possible old contents) in memory, unload it now
    {
        AssetPtr asset = FindAsset(assetRef);
        if (asset && asset->IsLoaded())
            asset->Unload();
    }
    
    uploadTransfer->EmitTransferCompleted();
    
    AssetUploaded(assetRef);
    
    // We've completed an asset upload transfer. See if there is an asset download transfer that is waiting
    // for this upload to complete. 
    
    currentUploadTransfers.erase(assetRef); // Note: this might kill the 'transfer' ptr if we were the last one to hold on to it. Don't dereference transfer below this.
    PendingDownloadRequestMap::iterator iter = pendingDownloadRequests.find(assetRef);
    if (iter != pendingDownloadRequests.end())
    {
        PendingDownloadRequest req = iter->second;

        AssetTransferPtr transfer = RequestAsset(req.assetRef, req.assetType);
        if (!transfer)
            return; ///\todo Evaluate the path to take here.
        transfer->Downloaded.Connect(req.transfer.Get(), &IAssetTransfer::EmitAssetDownloaded);
        transfer->Succeeded.Connect(req.transfer.Get(), &IAssetTransfer::EmitTransferSucceeded);
        transfer->Failed.Connect(req.transfer.Get(), &IAssetTransfer::EmitAssetFailed);
    }
}

void AssetAPI::AssetDependenciesCompleted(AssetTransferPtr transfer)
{    
    PROFILE(AssetAPI_AssetDependenciesCompleted);

    // Emit success for this transfer
    transfer->EmitTransferSucceeded();

    // This asset transfer has finished, remove it from the internal state.
    AssetTransferMap::iterator transferIter = FindTransferIterator(transfer.Get());
    if (transferIter != currentTransfers.end())
        currentTransfers.erase(transferIter);
    PendingDownloadRequestMap::iterator downloadIter = pendingDownloadRequests.find(transfer->source.ref);
    if (downloadIter != pendingDownloadRequests.end())
        pendingDownloadRequests.erase(downloadIter);
}

void AssetAPI::NotifyAssetDependenciesChanged(AssetPtr asset)
{
    PROFILE(AssetAPI_NotifyAssetDependenciesChanged);

    /// Delete all old stored asset dependencies for this asset.
    RemoveAssetDependencies(asset->Name());

    Vector<AssetReference> refs = asset->FindReferences();
    for(uint i = 0; i < refs.Size(); ++i)
    {
        // Turn named storage (and default storage) specifiers to absolute specifiers.
        String ref = refs[i].ref;
        if (ref.Empty())
            continue;

        // Remember this assetref for future lookup.
        assetDependencies.Push(MakePair(asset->Name(), ref));
    }
}

void AssetAPI::RequestAssetDependencies(AssetPtr asset)
{
    PROFILE(AssetAPI_RequestAssetDependencies);
    // Make sure we have most up-to-date internal view of the asset dependencies.
    NotifyAssetDependenciesChanged(asset);

    Vector<AssetReference> refs = asset->FindReferences();
    for(uint i = 0; i < refs.Size(); ++i)
    {
        AssetReference ref = refs[i];
        if (ref.ref.Empty())
            continue;

        AssetPtr existing = FindAsset(ref.ref);
        if (!existing || !existing->IsLoaded())
        {
//            LogDebug("Asset " + asset->ToString() + " depends on asset " + ref.ref + " (type=\"" + ref.type + "\") which has not been loaded yet. Requesting..");
            RequestAsset(ref);
        }
    }
}

void AssetAPI::RemoveAssetDependencies(String asset)
{
    PROFILE(AssetAPI_RemoveAssetDependencies);
    for(uint i = 0; i < assetDependencies.Size(); ++i)
        if (String::Compare(assetDependencies[i].first_.CString(), asset.CString(), false) == 0)
        {
            assetDependencies.Erase(assetDependencies.Begin() + i);
            --i;
        }
}

Vector<AssetPtr> AssetAPI::FindDependents(String dependee)
{
    PROFILE(AssetAPI_FindDependents);

    Vector<AssetPtr> dependents;
    for(uint i = 0; i < assetDependencies.Size(); ++i)
    {
        if (String::Compare(assetDependencies[i].second_.CString(), dependee.CString(), false) == 0)
        {
            AssetMap::iterator iter = assets.find(assetDependencies[i].first_);
            if (iter != assets.end())
                dependents.Push(iter->second);
        }
    }
    return dependents;
}

int AssetAPI::NumPendingDependencies(AssetPtr asset) const
{
    PROFILE(AssetAPI_NumPendingDependencies);
    int numDependencies = 0;

    Vector<AssetReference> refs = asset->FindReferences();
    for(uint i = 0; i < refs.Size(); ++i)
    {
        String ref = refs[i].ref;
        if (ref.Empty())
            continue;

        // We silently ignore this dependency if the asset type in question is disabled.
        if (dynamic_cast<NullAssetFactory*>(AssetTypeFactory(ResourceTypeForAssetRef(refs[i])).Get()))
            continue;

        AssetPtr existing = FindAsset(refs[i].ref);
        if (!existing)
        {
            // Not loaded, just mark the single one
            ++numDependencies;
        }
        else
        {
            // If asset is empty, count it as an unloaded dependency
            if (existing->IsEmpty())
                ++numDependencies;
            else
            {
                if (!existing->IsLoaded())
                    ++numDependencies;
                // Ask the dependencies of the dependency, we want all of the asset
                // down the chain to be loaded before we load the base asset
                // Note: if the dependency is unloaded, it may or may not be able to tell the dependencies correctly
                numDependencies += NumPendingDependencies(existing);
            }
        }
    }

    return numDependencies;
}

bool AssetAPI::HasPendingDependencies(AssetPtr asset) const
{
    PROFILE(AssetAPI_HasPendingDependencies);

    Vector<AssetReference> refs = asset->FindReferences();
    for(uint i = 0; i < refs.Size(); ++i)
    {
        if (refs[i].ref.Empty())
            continue;

        // We silently ignore this dependency if the asset type in question is disabled.
        if (dynamic_cast<NullAssetFactory*>(AssetTypeFactory(ResourceTypeForAssetRef(refs[i])).Get()))
            continue;

        AssetPtr existing = FindAsset(refs[i].ref);
        if (!existing) // Not loaded, just mark the single one
            return true;
        else
        {            
            if (existing->IsEmpty())
                return true; // If asset is empty, count it as an unloaded dependency
            else
            {
                if (!existing->IsLoaded())
                    return true;
                // Ask the dependencies of the dependency, we want all of the asset
                // down the chain to be loaded before we load the base asset
                // Note: if the dependency is unloaded, it may or may not be able to tell the dependencies correctly
                bool dependencyHasDependencies = HasPendingDependencies(existing);
                if (dependencyHasDependencies)
                    return true;
            }
        }
    }

    return false;
}

void AssetAPI::HandleAssetDiscovery(const String &assetRef, const String &assetType)
{
    HandleAssetDiscovery(assetRef, assetType, AssetStoragePtr());
}

void AssetAPI::HandleAssetDiscovery(const String &assetRef, const String &assetType, AssetStoragePtr storage)
{
    AssetPtr existing = FindAsset(assetRef);
    // If asset did not exist, create new empty asset
    if (!existing)
    {
        // If assettype is empty, guess it
        String newType = assetType;
        if (newType.Empty())
            newType = ResourceTypeForAssetRef(assetRef);
        CreateNewAsset(newType, assetRef, storage);
    }
    // If asset exists and is already loaded, forcibly request updated data
    else if (existing->IsLoaded())
        RequestAsset(assetRef, assetType, true);
}

void AssetAPI::HandleAssetDeleted(const String &assetRef)
{
    // If the asset is unloaded, delete it from memory. If it is loaded, it might be in use, so do nothing
    AssetPtr existing = FindAsset(assetRef);
    if (!existing)
        return;
    if (!existing->IsLoaded())
        ForgetAsset(existing, false);
}

void AssetAPI::EmitAssetDeletedFromStorage(const String &assetRef)
{
    AssetDeletedFromStorage.Emit(assetRef);
}

void AssetAPI::EmitAssetStorageAdded(AssetStoragePtr newStorage)
{
    // Connect to the asset storage's AssetChanged signal, so that we can create actual empty assets
    // from its refs whenever new assets are added to this storage from external sources.
    newStorage->AssetChanged.Connect(this, &AssetAPI::OnAssetChanged);
    AssetStorageAdded.Emit(newStorage);
}

HashMap<String, String> AssetAPI::ParseAssetStorageString(String storageString)
{
    storageString = storageString.Trimmed();
    // Swallow the right-most ';' if it exists to allow both forms "http://www.server.com/" and "http://www.server.com/;" below,
    // although the latter is somewhat odd form.
    if (storageString.EndsWith(";")) 
        storageString = storageString.Substring(0, storageString.Length()-1);

    // Treat simple strings of form "http://myserver.com/" as "src=http://myserver.com/".
    if (storageString.Find(';') == String::NPOS && storageString.Find('=') == String::NPOS)
        storageString = "src=" + storageString;

    HashMap<String, String> m;
    StringVector items = storageString.Split(';');
    foreach(String str, items)
    {
        StringVector keyValue = str.Split('=');
        if (keyValue.Size() > 2 || keyValue.Size() == 0)
        {
            LogError("Failed to parse asset storage string \"" + str + "\"!");
            return HashMap<String, String>();
        }
        if (keyValue.Size() == 1)
            keyValue.Push("1");
        m[keyValue[0]] = keyValue[1];
    }
    return m;
}

void AssetAPI::OnAssetLoaded(AssetPtr asset)
{
    PROFILE(AssetAPI_OnAssetLoaded);

    Vector<AssetPtr> dependents = FindDependents(asset->Name());
    for(uint i = 0; i < dependents.Size(); ++i)
    {
        AssetPtr dependent = dependents[i];

        // Notify the asset that one of its dependencies has now been loaded in.
        dependent->DependencyLoaded(asset);

        // Check if this dependency was the last one of the given asset's dependencies.
        AssetTransferMap::iterator iter = currentTransfers.find(dependent->Name());
        if (iter != currentTransfers.end())
        {
            AssetTransferPtr transfer = iter->second;
            if (!HasPendingDependencies(dependent))
                AssetDependenciesCompleted(transfer);
        }
    }
}

void AssetAPI::OnAssetDiskSourceChanged(const String &path)
{
    Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();

    for(AssetMap::iterator iter = assets.begin(); iter != assets.end(); ++iter)
    {
        String assetDiskSource = iter->second->DiskSource();
        /// \todo The compare may need path normalization/sanitation instead of a straight compare
        if (!assetDiskSource.Empty() && assetDiskSource == path && fileSystem->FileExists(assetDiskSource))
        {
            AssetPtr asset = iter->second;
            AssetStoragePtr storage = asset->AssetStorage();
            if (storage)
            {
                if (storage->HasLiveUpdate())
                {
                    LogInfo("AssetAPI: Detected file changes in '" + path + "', reloading asset.");
                    bool success = asset->LoadFromCache();
                    if (!success)
                        LogError("Failed to reload changed asset \"" + asset->ToString() + "\" from file \"" + path + "\"!");
                    else
                        LogDebug("Reloaded changed asset \"" + asset->ToString() + "\" from file \"" + path + "\".");
                }
                
                AssetDiskSourceChanged.Emit(asset);
            }
            else
                LogError("Detected file change for a storageless asset " + asset->Name());
        }
    }
}

void AssetAPI::OnAssetChanged(IAssetStorage* storage, String localName, String diskSource, IAssetStorage::ChangeType change)
{
    PROFILE(AssetAPI_OnAssetChanged);

    assert(storage);
    
    String assetRef = storage->GetFullAssetURL(localName);
    String assetType = ResourceTypeForAssetRef(assetRef);
    AssetPtr existing = FindAsset(assetRef);
    if (change == IAssetStorage::AssetCreate && existing)
    {
        LogDebug("AssetAPI: Received AssetCreate notification for existing and loaded asset " + assetRef + ". Handling this as AssetModify.");
        change = IAssetStorage::AssetModify;
    }

    switch(change)
    {
    case IAssetStorage::AssetCreate:
        {
            AssetPtr asset = CreateNewAsset(assetType, assetRef, AssetStoragePtr(storage));
            if (asset && !diskSource.Empty())
            {
                asset->SetDiskSource(diskSource);
                //bool cached = (assetCache && diskSource.contains(Cache()->CacheDirectory(), false));
                //asset->SetDiskSourceType(cached ? IAsset::Cached : IAsset::Original);
            }
        }
        break;
    case IAssetStorage::AssetModify:
        if (existing)
        {
            ///\todo Profile performance difference between LoadFromCache and RequestAsset
            if (existing->IsLoaded()) // Note: exists->LoadFromCache() could be used here as well.
                RequestAsset(assetRef, assetType, true); // If asset exists and is already loaded, forcibly request updated data
            else
                LogDebug("AssetAPI: Ignoring AssetModify notification for unloaded asset " + assetRef + ".");
        }
        else
            LogWarning("AssetAPI: Received AssetModify notification for non-existing asset " + assetRef + ".");
        break;
    case IAssetStorage::AssetDelete:
        if (existing)
        {
            if (existing->IsLoaded()) // Deleted asset is currently loaded. Invalidate disk source, but do not forget asset.
                existing->SetDiskSource("");
            else
                ForgetAsset(existing, false); // The asset should be already deleted; do not delete disk source.
        }
        else
            LogWarning("AssetAPI: Received AssetDelete notification for non-existing asset " + assetRef + ".");
        break;
    default:
        assert(false);
        break;
    }
}

bool LoadFileToVector(const String &filename, Vector<u8> &dst)
{
    // To open Urho files, need access to the Context. Abuse Framework static accessor
    Urho3D::Context* context = Framework::Instance()->GetContext();

    Urho3D::File file(context, filename, Urho3D::FILE_READ);
    if (!file.IsOpen())
    {
        LogError("AssetAPI::LoadFileToVector: Failed to open file '" + filename + "' for reading.");
        return false;
    }
    unsigned fileSize = file.GetSize();
    if (fileSize == 0)
    {
        // If the file size is 0 this is still considered a success.
        // Log out a warning for it either way as this is an unusual case.
        LogWarning(String("AssetAPI::LoadFileToVector: Source file '" + filename + "' exists but size is 0. Reading is reported to be successfull but read data is empty!"));
        return true;
    }
    dst.Resize(fileSize);
    unsigned numRead = file.Read(&dst[0], fileSize);
    file.Close();
    if (numRead < fileSize)
    {
        LogError(String("AssetAPI::LoadFileToVector: Failed to read full " + String(fileSize) + " bytes from file '" + filename + "', instead read " + String(numRead) + " bytes."));
        return false;
    }
    return true;
}
 
bool IsFileOfType(const String &filename, const char **suffixes, int numSuffixes)
{
    for(int i = 0;i < numSuffixes; ++i)
        if (filename.EndsWith(suffixes[i], false))
            return true;

    return false;
}

String AssetAPI::ResourceTypeForAssetRef(const AssetReference &ref) const
{
    String type = ref.type.Trimmed();
    if (!type.Empty())
        return type;
    return ResourceTypeForAssetRef(ref.ref);
}

String AssetAPI::ResourceTypeForAssetRef(String assetRef) const
{
    String filenameParsed;
    String subAssetFilename;
    ParseAssetRef(assetRef, 0, 0, 0, 0, 0, 0, &filenameParsed, &subAssetFilename);
    if (!subAssetFilename.Empty())
        filenameParsed = subAssetFilename;
    String filename = filenameParsed.Trimmed();

    // Query all registered asset factories if they provide this asset type.
    for(uint i=0; i<assetTypeFactories.Size(); ++i)
    {
        // Note that we cannot ask endsWith from 'Binary' factory as the extension
        // is an empty string. It will always return true and this factory should 
        // only be defaulted to if nothing else can provide the queried file extension.
        if (assetTypeFactories[i]->Type() == "Binary")
            continue;

        foreach (String extension, assetTypeFactories[i]->TypeExtensions())
            if (filename.EndsWith(extension, false))
                return assetTypeFactories[i]->Type();
    }

    // Query all registered bundle factories if they provide this asset type.
    for(uint i=0; i<assetBundleTypeFactories.Size(); ++i)
        foreach (String extension, assetBundleTypeFactories[i]->TypeExtensions())
            if (filename.EndsWith(extension, false))
                return assetBundleTypeFactories[i]->Type();
                
    /** @todo Make these hardcoded ones go away and move to the provider when provided (like above). 
        Seems the resource types have leaked here without the providers being in the code base.
        Where ever the code might be, remove these once the providers have been updated to return
        the type extensions correctly. */
    /// @todo We don't support QML, remove the following
    /*
    if (filename.EndsWith(".qml", false) || filename.EndsWith(".qmlzip", false))
        return "QML";
    if (filename.EndsWith(".pdf", false))
        return "PdfAsset";
    const char *openDocFileTypes[] = { ".odt", ".doc", ".rtf", ".txt", ".docx", ".docm", ".ods", ".xls", ".odp", ".ppt", ".odg" };
    if (IsFileOfType(filename, openDocFileTypes, NUMELEMS(openDocFileTypes)))
        return "DocAsset";
    */

    // Could not resolve the asset extension to any registered asset factory. Return Binary type.
    return "Binary";
}

String AssetAPI::SanitateAssetRef(const String& input)
{
    String ret = input;
    if (ret.Contains('$'))
        return ret; // Already sanitated

    ret.Replace(":", "$1");
    ret.Replace("/", "$2");
    ret.Replace("\\", "$3");
    ret.Replace("*", "$4");
    ret.Replace("?", "$5");
    ret.Replace("\"", "$6");
    ret.Replace("'", "$7");
    ret.Replace("<", "$8");
    ret.Replace(">", "$9");
    ret.Replace("|", "$0");
    return ret;
}

String AssetAPI::DesanitateAssetRef(const String& input)
{
    String ret = input;
    ret.Replace("$1", ":");
    ret.Replace("$2", "/");
    ret.Replace("$3", "\\");
    ret.Replace("$4", "*");
    ret.Replace("$5", "?");
    ret.Replace("$6", "\"");
    ret.Replace("$7", "'");
    ret.Replace("$8", "<");
    ret.Replace("$9", ">");
    ret.Replace("$0", "|");
    return ret;
}

std::string AssetAPI::SanitateAssetRef(const std::string& input)
{
    return std::string(SanitateAssetRef(String(input.c_str())).CString());
}

std::string AssetAPI::DesanitateAssetRef(const std::string& input)
{
    return std::string(DesanitateAssetRef(String(input.c_str())).CString());
}

bool CopyAssetFile(const String &sourceFile, const String &destFile)
{
    assert(!sourceFile.Trimmed().Empty());
    assert(!destFile.Trimmed().Empty());

    // To open Urho files, need access to the Context. Abuse Framework static accessor
    Urho3D::Context* context = Framework::Instance()->GetContext();
    Urho3D::FileSystem* fileSystem = context->GetSubsystem<Urho3D::FileSystem>();
    return fileSystem->Copy(sourceFile, destFile);
}

bool SaveAssetFromMemoryToFile(const u8 *data, uint numBytes, const String &destFile)
{
    // To open Urho files, need access to the Context. Abuse Framework static accessor
    Urho3D::Context* context = Framework::Instance()->GetContext();

    assert(data);
    assert(!destFile.Trimmed().Empty());

    Urho3D::File asset_out(context, destFile, Urho3D::FILE_WRITE);
    if (!asset_out.IsOpen())
    {
        LogError("Could not open output asset file \"" + destFile + "\"");
        return false;
    }

    asset_out.Write(data, numBytes);
    asset_out.Close();
    
    return true;
}

}
