//
//  CCEffect_Private.h
//  cocos2d-ios
//
//  Created by Oleg Osin on 4/10/14.
//
//

#import "CCEffect.h"
#import "CCEffectStackProtocol.h"


#ifndef BLUR_OPTIMIZED_RADIUS_MAX
#define BLUR_OPTIMIZED_RADIUS_MAX 6UL
#endif

extern NSString * const CCShaderUniformPreviousPassTexture;
extern NSString * const CCShaderUniformTexCoord1Center;
extern NSString * const CCShaderUniformTexCoord1Extents;
extern NSString * const CCShaderUniformTexCoord2Center;
extern NSString * const CCShaderUniformTexCoord2Extents;
extern NSString * const CCEffectDefaultInitialInputSnippet;
extern NSString * const CCEffectDefaultInputSnippet;


typedef NS_ENUM(NSUInteger, CCEffectFunctionStitchFlags)
{
    CCEffectFunctionStitchBefore     = 1 << 0,
    CCEffectFunctionStitchAfter      = 1 << 1,
    CCEffectFunctionStitchBoth       = (CCEffectFunctionStitchBefore | CCEffectFunctionStitchAfter),
};

typedef NS_ENUM(NSUInteger, CCEffectPrepareStatus)
{
    CCEffectPrepareNothingToDo   = 0,
    CCEffectPrepareFailure       = 1,
    CCEffectPrepareSuccess       = 2,
};

typedef NS_ENUM(NSUInteger, CCEffectTexCoordMapping)
{
    CCEffectTexCoordMapMainTex              = 0,
    CCEffectTexCoordMapPreviousPassTex      = 1,
    CCEffectTexCoordMapCustomTex            = 2,
    CCEffectTexCoordMapCustomTexNoTransform = 3
};

@interface CCEffectFunction : NSObject

@property (nonatomic, readonly) NSString* body;
@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSArray* inputs;
@property (nonatomic, readonly) NSString* inputString;
@property (nonatomic, readonly) NSString* returnType;
@property (nonatomic, readonly) NSString* function;

-(id)initWithName:(NSString*)name body:(NSString*)body inputs:(NSArray*)inputs returnType:(NSString*)returnType;
+(id)functionWithName:(NSString*)name body:(NSString*)body inputs:(NSArray*)inputs returnType:(NSString*)returnType;

-(NSString*)callStringWithInputs:(NSArray*)inputs;

@end

@interface CCEffectFunctionInput : NSObject

@property (nonatomic, readonly) NSString* type;
@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSString* initialSnippet;
@property (nonatomic, readonly) NSString* snippet;

-(id)initWithType:(NSString*)type name:(NSString*)name initialSnippet:(NSString*)initialSnippet snippet:(NSString*)snippet;
+(id)inputWithType:(NSString*)type name:(NSString*)name initialSnippet:(NSString*)initialSnippet snippet:(NSString*)snippet;

@end

@interface CCEffectUniform : NSObject

@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSString* type;
@property (nonatomic, readonly) NSString* declaration;
@property (nonatomic, readonly) NSValue* value;

-(id)initWithType:(NSString*)type name:(NSString*)name value:(NSValue*)value;
+(id)uniform:(NSString*)type name:(NSString*)name value:(NSValue*)value;

@end

@interface CCEffectVarying : NSObject

@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSString* type;
@property (nonatomic, readonly) NSString* declaration;
@property (nonatomic, readonly) NSInteger count;

-(id)initWithType:(NSString*)type name:(NSString*)name;
-(id)initWithType:(NSString*)type name:(NSString*)name count:(NSInteger)count;
+(id)varying:(NSString*)type name:(NSString*)name;
+(id)varying:(NSString*)type name:(NSString*)name count:(NSInteger)count;

@end



@interface CCEffectRenderPassInputs : NSObject

@property (nonatomic, assign) NSInteger renderPassId;
@property (nonatomic, strong) CCRenderer* renderer;
@property (nonatomic, strong) CCSprite *sprite;
@property (nonatomic, assign) CCSpriteVertexes verts;
@property (nonatomic, strong) CCTexture *previousPassTexture;
@property (nonatomic, assign) GLKMatrix4 transform;
@property (nonatomic, assign) GLKMatrix4 ndcToNodeLocal;
@property (nonatomic, assign) GLKVector2 texCoord1Center;
@property (nonatomic, assign) GLKVector2 texCoord1Extents;
@property (nonatomic, assign) GLKVector2 texCoord2Center;
@property (nonatomic, assign) GLKVector2 texCoord2Extents;
@property (nonatomic, strong) NSMutableDictionary* shaderUniforms;
@property (nonatomic, assign) BOOL needsClear;

@end


@class CCEffectRenderPass;

typedef void (^CCEffectRenderPassBeginBlock)(CCEffectRenderPass *pass, CCEffectRenderPassInputs *passInputs);
typedef void (^CCEffectRenderPassUpdateBlock)(CCEffectRenderPass *pass, CCEffectRenderPassInputs *passInputs);
typedef void (^CCEffectRenderPassEndBlock)(CCEffectRenderPass *pass, CCEffectRenderPassInputs *passInputs);

@interface CCEffectRenderPass : NSObject

@property (nonatomic, readonly) NSUInteger indexInEffect;
@property (nonatomic, assign) CCEffectTexCoordMapping texCoord1Mapping;
@property (nonatomic, assign) CCEffectTexCoordMapping texCoord2Mapping;
@property (nonatomic, strong) CCBlendMode* blendMode;
@property (nonatomic, strong) CCShader* shader;
@property (nonatomic, copy) NSArray* beginBlocks;
@property (nonatomic, copy) NSArray* updateBlocks;
@property (nonatomic, copy) NSArray* endBlocks;
@property (nonatomic, copy) NSString *debugLabel;

-(id)initWithIndex:(NSUInteger)indexInEffect;

-(void)begin:(CCEffectRenderPassInputs *)passInputs;
-(void)update:(CCEffectRenderPassInputs *)passInputs;
-(void)end:(CCEffectRenderPassInputs *)passInputs;
-(void)enqueueTriangles:(CCEffectRenderPassInputs *)passInputs;

@end


@class CCEffectImpl;


@interface CCEffect ()

@property (nonatomic, strong) CCEffectImpl *effectImpl;

@property (nonatomic, readonly) BOOL supportsDirectRendering;
@property (nonatomic, readonly) NSUInteger renderPassCount;
@property (nonatomic, weak) id<CCEffectStackProtocol> owningStack;

-(CCEffectPrepareStatus)prepareForRenderingWithSprite:(CCSprite *)sprite;;
-(CCEffectRenderPass *)renderPassAtIndex:(NSUInteger)passIndex;

@end


@interface CCEffectImpl : NSObject

@property (nonatomic, copy) NSString *debugName;

@property (nonatomic, readonly) BOOL supportsDirectRendering;

@property (nonatomic, readonly) CCShader* shader;
@property (nonatomic, readonly) NSMutableDictionary* shaderUniforms;
@property (nonatomic, readonly) NSMutableDictionary* uniformTranslationTable;
@property (nonatomic, readonly) NSArray* vertexFunctions;
@property (nonatomic, readonly) NSArray* fragmentFunctions;
@property (nonatomic, readonly) NSArray* fragmentUniforms;
@property (nonatomic, readonly) NSArray* vertexUniforms;
@property (nonatomic, readonly) NSArray* varyingVars;

@property (nonatomic, readonly) NSArray* renderPasses;
@property (nonatomic, assign) CCEffectFunctionStitchFlags stitchFlags;

@property (nonatomic, readonly) BOOL firstInStack;


-(id)initWithRenderPasses:(NSArray *)renderPasses fragmentFunctions:(NSArray*)fragmentFunctions vertexFunctions:(NSArray*)vertexFunctions fragmentUniforms:(NSArray*)fragmentUniforms vertexUniforms:(NSArray*)vertexUniforms varyings:(NSArray*)varyings firstInStack:(BOOL)firstInStack;
-(id)initWithRenderPasses:(NSArray *)renderPasses shaderUniforms:(NSMutableDictionary *)uniforms;

-(CCEffectPrepareStatus)prepareForRenderingWithSprite:(CCSprite *)sprite;
-(CCEffectRenderPass *)renderPassAtIndex:(NSUInteger)passIndex;

-(BOOL)stitchSupported:(CCEffectFunctionStitchFlags)stitch;

+ (NSSet *)defaultEffectFragmentUniformNames;
+ (NSSet *)defaultEffectVertexUniformNames;

@end

