#pragma once

/// \file

#include <Foundation/Communication/Message.h>
#include <Foundation/Reflection/Reflection.h>

#include <Core/World/Declarations.h>

class ezWorldWriter;
class ezWorldReader;

/// \brief Base class of all component types.
///
/// Derive from this class to implement custom component types. Also add the EZ_DECLARE_COMPONENT_TYPE macro to your class declaration.
/// Also add a EZ_BEGIN_COMPONENT_TYPE/EZ_END_COMPONENT_TYPE block to a cpp file. In that block you can add reflected members or message handlers.
/// Note that every component type needs a corresponding manager type. Take a look at ezComponentManagerSimple for a simple manager 
/// implementation that calls an update method on its components every frame.
/// To create a component instance call CreateComponent on the corresponding manager. Never store a direct pointer to a component but store a 
/// component handle instead.
class EZ_CORE_DLL ezComponent : public ezReflectedClass
{
  EZ_ADD_DYNAMIC_REFLECTION(ezComponent, ezReflectedClass);

protected:
  /// \brief Keep the constructor private or protected in derived classes, so it cannot be called manually.
  ezComponent();
  virtual ~ezComponent();

public:
  /// \brief Returns whether this component is dynamic and thus can only be attached to dynamic game objects.
  bool IsDynamic() const;

  /// \brief Sets the active state of the component. Note that it is up to the manager if he differentiates between active and inactive components.
  void SetActive(bool bActive);

  /// \brief Activates the component. Note that it is up to the manager if he differentiates between active and inactive components.
  void Activate();

  /// \brief Deactivates the component.
  void Deactivate();

  /// \brief Returns whether this component is active.
  bool IsActive() const;

  /// \brief Returns the corresponding manager for this component.
  ezComponentManagerBase* GetManager() const;

  /// \brief Returns the owner game object if the component is attached to one or nullptr.
  ezGameObject* GetOwner();

  /// \brief Returns the owner game object if the component is attached to one or nullptr.
  const ezGameObject* GetOwner() const;

  /// \brief Returns the corresponding world for this componenet.
  ezWorld* GetWorld();

  /// \brief Returns the corresponding world for this componenet.
  const ezWorld* GetWorld() const;

  /// \brief Returns a handle to this component.
  ezComponentHandle GetHandle() const;

  /// \brief Returns the type id corresponding to this component type.
  static ezUInt16 TypeId();

  ezUInt32 m_uiEditorPickingID;

  /// \brief Override this to save the current state of the component to the given stream.
  virtual void SerializeComponent(ezWorldWriter& stream) const = 0;

  /// \brief Override this to load the current state of the component from the given stream.
  ///
  /// The active state will be automatically serialized. The 'initialized' state is not serialized, all components
  /// will be initialized after creation, even if they were already in an initialized state when they were serialized.
  virtual void DeserializeComponent(ezWorldReader& stream) = 0;

protected:
  friend class ezWorld;
  friend class ezGameObject;
  friend class ezComponentManagerBase;

  template <typename T>
  ezComponentHandle GetHandle() const;

  ezBitflags<ezObjectFlags> m_ComponentFlags;

  enum class Initialization
  {
    Done,
    RequiresInit2
  };

  virtual ezUInt16 GetTypeId() const = 0;

  /// \brief This method is called at the start of the next world update. The global position will be computed before initialization.
  virtual Initialization Initialize() { return Initialization::Done; }

  /// \brief If Initialize() returned Initialization::RequiresInit2, this function is called after Initialize() has been called on ALL other components during the world update start.
  ///        This allows to access other components during Initialize2().
  virtual void Initialize2() {}

  /// \brief This method is called before the destructor. A derived type can override this method to do common de-initialization work.
  virtual void Deinitialize() {}

  /// \brief Returns whether this component is initialized. Internal method.
  bool IsInitialized() const;

  /// \brief This method is called when the component is attached to a game object. At this point the owner pointer is already set. A derived type can override this method to do additional work.
  virtual void OnAfterAttachedToObject() {}

  /// \brief This method is called when the component is detached from a game object. At this point the owner pointer is still set. A derived type can override this method to do additional work.
  virtual void OnBeforeDetachedFromObject() {}

private:
  void OnMessage(ezMessage& msg);
  void OnMessage(ezMessage& msg) const;

  ezGenericComponentId m_InternalId;

  ezComponentManagerBase* m_pManager;
  ezGameObject* m_pOwner;

  static ezUInt16 TYPE_ID;
};

#include <Core/World/Implementation/Component_inl.h>
