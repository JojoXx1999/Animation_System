#include "Animation_Parser/3D/Headers/ragdoll_handler.h"

RagdollHandler::RagdollHandler() :
	dynamics_world_(NULL)
{
}

RagdollHandler::~RagdollHandler()
{

}

std::string model_name("xbot");

void RagdollHandler::InitRagdoll(std::string filename, gef::SkinnedMeshInstance* player, gef::Renderer3D* renderer_3d, gef::Platform& platform)
{
	primitive_builder_ = new PrimitiveBuilder(platform);
	//primitive_renderer_ = new PrimitiveRenderer(platform);

	InitPhysicsWorld(renderer_3d);
	CreateRigidBodies();

	if (player->bind_pose().skeleton())
	{
		m_ragdoll = new Ragdoll();
		m_ragdoll->set_scale_factor(0.01f);

		std::string ragdoll_filename;
		ragdoll_filename = model_name + "/ragdoll.bullet";

		m_ragdoll->Init(player->bind_pose(), dynamics_world_, ragdoll_filename.c_str());
	}

	m_is_ragdoll_simulating = false;
}

void RagdollHandler::CleanUp()
{
	CleanUpRagdoll();
	CleanUpPhysicsWorld();
	CleanUpRigidBodies();

	delete primitive_builder_;
	primitive_builder_ = NULL;
}

void RagdollHandler::CleanUpRagdoll()
{
	delete m_ragdoll;
	m_ragdoll = NULL;
}

void RagdollHandler::Update(gef::SkinnedMeshInstance* player, gef::SkeletonPose* clip_player, bool toggle, float speed)
{
	if (player && m_ragdoll)
	{
		if (m_is_ragdoll_simulating && !toggle)
		{
			m_ragdoll->UpdatePoseFromRagdoll();
			player->UpdateBoneMatrices(m_ragdoll->pose());
		}
		else if (toggle)
		{
			toggle = false;
			m_ragdoll->set_pose(*clip_player);
			//clip_player.speed_ = speed;
			m_ragdoll->UpdateRagdollFromPose(2);
		}
	}
}

void RagdollHandler::InitPhysicsWorld(gef::Renderer3D* renderer_3d)
{
	/// collision configuration contains default setup for memory , collision setup . Advanced users can create their own configuration .
	btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();

	/// use the default collision dispatcher . For parallel processing you can use a diffent dispatcher(see Extras / BulletMultiThreaded)
	dispatcher_ = new btCollisionDispatcher(collision_configuration);

	/// btDbvtBroadphase is a good general purpose broadphase . You can also try out btAxis3Sweep .
	overlapping_pair_cache_ = new btDbvtBroadphase();

	/// the default constraint solver . For parallel processing you can use a different solver (see Extras / BulletMultiThreaded)
	solver_ = new btSequentialImpulseConstraintSolver;

	dynamics_world_ = new btDiscreteDynamicsWorld(dispatcher_, overlapping_pair_cache_, solver_, collision_configuration);
	dynamics_world_->setGravity(btVector3(0, -9.8f, 0));

	/*
	debug_drawer_ = new GEFDebugDrawer(renderer_3d);
	debug_drawer_->setDebugMode(btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawFrames);
	dynamics_world_->setDebugDrawer(debug_drawer_);
	*/
}

void RagdollHandler::CleanUpPhysicsWorld()
{

	for (int i = dynamics_world_->getNumConstraints() - 1; i >= 0; i--)
	{
		btTypedConstraint* constraint = dynamics_world_->getConstraint(i);
		dynamics_world_->removeConstraint(constraint);
		delete constraint;
	}


	// remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamics_world_->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamics_world_->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamics_world_->removeCollisionObject(obj);
		delete obj;
	}

	// delete collision shapes
	for (int j = 0; j < collision_shapes_.size(); j++)
	{
		btCollisionShape* shape = collision_shapes_[j];
		collision_shapes_[j] = 0;
		delete shape;
	}

	// delete dynamics world
	delete dynamics_world_;

	// delete solver
	delete solver_;

	// delete broadphase
	delete overlapping_pair_cache_;

	// delete dispatcher
	delete dispatcher_;

	dynamics_world_ = NULL;
	solver_ = NULL;
	overlapping_pair_cache_ = NULL;
	dispatcher_ = NULL;

	// next line is optional : it will be cleared by the destructor when the array goes out of scope
	collision_shapes_.clear();
}
void RagdollHandler::UpdatePhysicsWorld(float delta_time)
{
	const btScalar simulation_time_step = 1.0f / 60.0f;
	const int max_sub_steps = 1;
	dynamics_world_->stepSimulation(simulation_time_step, max_sub_steps);
}

void RagdollHandler::CreateRigidBodies()
{
	//the ground is a cube of side 100 at position y = 0.
	{
		btVector3 groundHalfExtents(btScalar(50.), btScalar(1.), btScalar(50.));
		btCollisionShape* groundShape = new btBoxShape(groundHalfExtents);

		collision_shapes_.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -groundHalfExtents.y(), 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		//add the body to the dynamics world
		dynamics_world_->addRigidBody(body);

		floor_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(groundHalfExtents.x(), groundHalfExtents.y(), groundHalfExtents.z()));
		floor_gfx_.set_mesh(floor_mesh_);
		floor_gfx_.set_transform(btTransform2Matrix(groundTransform));
	}

	//	if(0)
	{
		//create a dynamic rigidbody

		const btScalar  sphereRadius = 1.f;
		btCollisionShape* colShape = new btSphereShape(sphereRadius);

		collision_shapes_.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamics_world_->addRigidBody(body);

		sphere_rb_ = body;
		sphere_mesh_ = primitive_builder_->CreateSphereMesh(sphereRadius, 20, 20);
		sphere_gfx_.set_mesh(sphere_mesh_);
		sphere_gfx_.set_transform(btTransform2Matrix(startTransform));
	}
}

void RagdollHandler::CleanUpRigidBodies()
{
	delete sphere_mesh_;
	sphere_mesh_ = NULL;
	delete floor_mesh_;
	floor_mesh_ = NULL;
}

void RagdollHandler::UpdateRigidBodies()
{
	if (sphere_rb_)
	{
		btTransform world_transform;
		sphere_rb_->getMotionState()->getWorldTransform(world_transform);
		sphere_gfx_.set_transform(btTransform2Matrix(world_transform));
	}
}

void RagdollHandler::Render(gef::Renderer3D* renderer_3d)
{
	//renderer_3d->DrawMesh(floor_gfx_);
	//renderer_3d->DrawMesh(sphere_gfx_);

	if (dynamics_world_)
		dynamics_world_->debugDrawWorld();
}