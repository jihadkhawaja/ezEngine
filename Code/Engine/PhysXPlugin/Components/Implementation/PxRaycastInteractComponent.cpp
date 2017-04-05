#include <PCH.h>
#include <PhysXPlugin/Components/PxRaycastInteractComponent.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <GameEngine/Interfaces/PhysicsWorldModule.h>
#include <PhysXPlugin/Components/PxCharacterProxyComponent.h>

//////////////////////////////////////////////////////////////////////////

EZ_IMPLEMENT_MESSAGE_TYPE(ezPxRaycastInteractComponent_Execute);
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezPxRaycastInteractComponent_Execute, 1, ezRTTIDefaultAllocator<ezPxRaycastInteractComponent_Execute>)
EZ_END_DYNAMIC_REFLECTED_TYPE

//////////////////////////////////////////////////////////////////////////

EZ_BEGIN_COMPONENT_TYPE(ezPxRaycastInteractComponent, 1)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MEMBER_PROPERTY("CollisionLayer", m_uiCollisionLayer)->AddAttributes(new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
    EZ_MEMBER_PROPERTY("MaxDistance", m_fMaxDistance)->AddAttributes(new ezClampValueAttribute(0, ezVariant()), new ezDefaultValueAttribute(1.0f)),
    EZ_MEMBER_PROPERTY("UserMessage", m_sUserMessage),
  }
  EZ_END_PROPERTIES
  EZ_BEGIN_MESSAGEHANDLERS
  {
    EZ_MESSAGE_HANDLER(ezPxRaycastInteractComponent_Execute, Execute),
  }
  EZ_END_MESSAGEHANDLERS
  EZ_BEGIN_ATTRIBUTES
  {
    new ezCategoryAttribute("Gameplay"),
    new ezDirectionVisualizerAttribute(ezBasisAxis::PositiveX, 0.2f, ezColor::LightGreen),
  }
  EZ_END_ATTRIBUTES
}
EZ_END_DYNAMIC_REFLECTED_TYPE

ezPxRaycastInteractComponent::ezPxRaycastInteractComponent() { }
ezPxRaycastInteractComponent::~ezPxRaycastInteractComponent() { }

void ezPxRaycastInteractComponent::SerializeComponent(ezWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);
  auto& s = stream.GetStream();

  s << m_uiCollisionLayer;
  s << m_fMaxDistance;
  s << m_sUserMessage;
}

void ezPxRaycastInteractComponent::DeserializeComponent(ezWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  //const ezUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());
  auto& s = stream.GetStream();

  s >> m_uiCollisionLayer;
  s >> m_fMaxDistance;
  s >> m_sUserMessage;
}

void ezPxRaycastInteractComponent::Execute(ezPxRaycastInteractComponent_Execute& msg)
{
  /// \todo The manager initialization doesn't work properly

  //ezPhysicsWorldModuleInterface* pPhysicsInterface = GetManager()->m_pPhysicsInterface;
  ezPhysicsWorldModuleInterface* pPhysicsInterface = GetWorld()->GetModuleOfBaseType<ezPhysicsWorldModuleInterface>();

  const ezVec3 vDirection = GetOwner()->GetGlobalRotation() * ezVec3(1, 0, 0);

  ezUInt32 uiIgnoreShapeID = ezInvalidIndex;

  ezGameObject* pCur = GetOwner();
  while (pCur)
  {
    ezPxCharacterProxyComponent* pProxy;
    if (pCur->TryGetComponentOfBaseType<ezPxCharacterProxyComponent>(pProxy))
    {
      uiIgnoreShapeID = pProxy->GetShapeId();
      break;
    }

    pCur = pCur->GetParent();
  }

  ezPhysicsHitResult res;
  if (!pPhysicsInterface->CastRay(GetOwner()->GetGlobalPosition(), vDirection, m_fMaxDistance, m_uiCollisionLayer, res, uiIgnoreShapeID))
    return;

  SendMessage(res);
}

void ezPxRaycastInteractComponent::SendMessage(const ezPhysicsHitResult& hit)
{
  ezUserTriggerMessage msg;
  msg.m_sMessage = m_sUserMessage;

  GetWorld()->SendMessage(hit.m_hGameObject, msg, ezObjectMsgRouting::ToEventHandler);
}

//////////////////////////////////////////////////////////////////////////


ezPxRaycastInteractComponentManager::ezPxRaycastInteractComponentManager(ezWorld* pWorld)
  : ezComponentManager<class ezPxRaycastInteractComponent, ezBlockStorageType::Compact>(pWorld)
  , m_pPhysicsInterface(nullptr)
{
}

void ezPxRaycastInteractComponentManager::Initialize()
{
  m_pPhysicsInterface = GetWorld()->GetModuleOfBaseType<ezPhysicsWorldModuleInterface>();
}