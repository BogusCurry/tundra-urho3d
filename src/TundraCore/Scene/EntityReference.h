// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#include "TundraCoreApi.h"
#include "SceneFwd.h"

namespace Tundra
{
/// Represents a reference to an entity, either by name or ID.
/** This structure can be used as a parameter type to an EC attribute. */
struct TUNDRACORE_API EntityReference
{
    EntityReference() {}

    explicit EntityReference(const String &entityName) : ref(entityName.Trimmed()) {}
    explicit EntityReference(entity_id_t id) : ref(String(id)) {}

    /// Set from an entity. If the name is unique within its parent scene, the name will be set, otherwise ID.
    void Set(EntityPtr entity);
    void Set(Entity* entity);

    /// Lookup an entity from the scene according to the ref. Return null pointer if not found
    EntityPtr Lookup(Scene* scene) const;
    /// Lookup a parent entity from the scene according to the ref. If the ref is empty, use the entity's parent (default value).
    EntityPtr LookupParent(Entity* entity) const;

    /// Returns if @c entity matches this EntityReference.
    bool Matches(Entity *entity) const;

    /// Return whether the ref does not refer to an entity
    bool IsEmpty() const;

    bool operator ==(const EntityReference &rhs) const { return this->ref == rhs.ref; }

    bool operator !=(const EntityReference &rhs) const { return !(*this == rhs); }

    bool operator <(const EntityReference &rhs) const { return ref < rhs.ref; }

    /// The entity pointed to. This can be either an entity ID, or an entity name
    String ref;
};

}
