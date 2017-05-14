static void InitPlayer(game_state* GameState, entity* Player)
{
    Player->Name = "Player";
    Player->Type = Entity_Player;
    Player->Player.WalkingSpeed = 10.0f;
    
    animation* IdleAnimation = (animation*)malloc(sizeof(animation));
    
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", IdleAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(IdleAnimation->Name, *IdleAnimation));
    
    animation* WalkingAnimation = (animation*)malloc(sizeof(animation));
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", WalkingAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(WalkingAnimation->Name, *WalkingAnimation));
    
    animation* AttackingAnimation = (animation*)malloc(sizeof(animation));
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", AttackingAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(AttackingAnimation->Name, *AttackingAnimation));
    
    render_entity PlayerRenderEntity = { };
    PlayerRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    
    PlayerRenderEntity.TextureHandle = GameState->RenderState.PlayerTexture;
    
    Player->RenderEntity = PlayerRenderEntity;
    Player->Rotation = glm::vec3(0, 0, 0);
    Player->Scale = glm::vec3(2, 2, 0);
    Player->Velocity = glm::vec2(0,0);
    
    collision_AABB CollisionAABB;
    Player->Center = glm::vec2(0.5f, 0.950f);
    CollisionAABB.Center = glm::vec2(Player->Position.x + Player->Center.x * Player->Scale.x,
                                     Player->Position.y + Player->Center.y * Player->Scale.y);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    Player->CollisionAABB = CollisionAABB;
    
    Player->EntityIndex = GameState->EntityCount;
    GameState->Entities[GameState->EntityCount++] = *Player;
    
    entity PlayerWeapon;
    PlayerWeapon.RenderEntity.Rendered = false;
    PlayerWeapon.Name = "Player weapon";
    PlayerWeapon.Type = Entity_PlayerWeapon;
    
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0.5, 0.5);
    CollisionAABB3.Extents = glm::vec2(0.5f,0.5f);
    CollisionAABB3.IsTrigger = true;
    PlayerWeapon.CollisionAABB = CollisionAABB3;
    PlayerWeapon.Rotation = glm::vec3(0, 0, 0);
    PlayerWeapon.Scale = glm::vec3(1, 1, 0);
    
    PlayerWeapon.EntityIndex = GameState->EntityCount;
    GameState->Entities[GameState->EntityCount++] = PlayerWeapon;
}


