//
//  CCEffectContrast.m
//  cocos2d-ios
//
//  Created by Thayer J Andrews on 5/7/14.
//
//

#import "CCEffectContrast.h"
#import "CCEffect_Private.h"
#import "CCRenderer.h"
#import "CCTexture.h"

static float conditionContrast(float contrast);


@interface CCEffectContrastImpl : CCEffectImpl

@property (nonatomic, strong) NSNumber *conditionedContrast;

@end


@implementation CCEffectContrastImpl

-(id)initWithContrast:(float)contrast
{
    CCEffectUniform* uniformContrast = [CCEffectUniform uniform:@"float" name:@"u_contrast" value:[NSNumber numberWithFloat:1.0f]];
    
    NSArray *fragFunctions = [CCEffectContrastImpl buildFragmentFunctions];
    NSArray *renderPasses = [self buildRenderPasses];

    if((self = [super initWithRenderPasses:renderPasses fragmentFunctions:fragFunctions vertexFunctions:nil fragmentUniforms:@[uniformContrast] vertexUniforms:nil varyings:nil firstInStack:YES]))
    {
        _conditionedContrast = [NSNumber numberWithFloat:conditionContrast(contrast)];

        self.debugName = @"CCEffectContrastImpl";
    }
    return self;
}

+ (NSArray *)buildFragmentFunctions
{
    CCEffectFunctionInput *input = [[CCEffectFunctionInput alloc] initWithType:@"vec4" name:@"inputValue" initialSnippet:CCEffectDefaultInitialInputSnippet snippet:CCEffectDefaultInputSnippet];

    NSString* effectBody = CC_GLSL(
                                   vec3 offset = vec3(0.5) * inputValue.a;
                                   return vec4(((inputValue.rgb - offset) * vec3(u_contrast) + offset), inputValue.a);
                                   );
    
    CCEffectFunction* fragmentFunction = [[CCEffectFunction alloc] initWithName:@"contrastEffect" body:effectBody inputs:@[input] returnType:@"vec4"];
    return @[fragmentFunction];
}

- (NSArray *)buildRenderPasses
{
    __weak CCEffectContrastImpl *weakSelf = self;
    
    CCEffectRenderPass *pass0 = [[CCEffectRenderPass alloc] init];
    pass0.debugLabel = @"CCEffectContrast pass 0";
    pass0.shader = self.shader;
    pass0.beginBlocks = @[[^(CCEffectRenderPass *pass, CCEffectRenderPassInputs *passInputs){
        
        passInputs.shaderUniforms[CCShaderUniformMainTexture] = passInputs.previousPassTexture;
        passInputs.shaderUniforms[CCShaderUniformPreviousPassTexture] = passInputs.previousPassTexture;
        passInputs.shaderUniforms[CCShaderUniformTexCoord1Center] = [NSValue valueWithGLKVector2:passInputs.texCoord1Center];
        passInputs.shaderUniforms[CCShaderUniformTexCoord1Extents] = [NSValue valueWithGLKVector2:passInputs.texCoord1Extents];

        passInputs.shaderUniforms[weakSelf.uniformTranslationTable[@"u_contrast"]] = weakSelf.conditionedContrast;
    } copy]];
    
    return @[pass0];
}

-(void)setContrast:(float)contrast
{
    _conditionedContrast = [NSNumber numberWithFloat:conditionContrast(contrast)];
}

@end


@implementation CCEffectContrast

-(id)init
{
    return [self initWithContrast:0.0f];
}

-(id)initWithContrast:(float)contrast
{
    if((self = [super init]))
    {
        _contrast = contrast;

        self.effectImpl = [[CCEffectContrastImpl alloc] initWithContrast:contrast];
        self.debugName = @"CCEffectContrast";
    }
    return self;
}

+(id)effectWithContrast:(float)contrast
{
    return [[self alloc] initWithContrast:contrast];
}

-(void)setContrast:(float)contrast
{
    _contrast = contrast;
    
    CCEffectContrastImpl *contrastImpl = (CCEffectContrastImpl *)self.effectImpl;
    [contrastImpl setContrast:contrast];
}

@end


float conditionContrast(float contrast)
{
    NSCAssert((contrast >= -1.0) && (contrast <= 1.0), @"Supplied contrast out of range [-1..1].");

    // Yes, this value is somewhat magical. It was arrived at experimentally by comparing
    // our results at min and max contrast (-1 and 1 respectively) with the results from
    // various image editing applications at their own min and max contrast values.
    static const float kContrastBase = 4.0f;
    
    float clampedExp = clampf(contrast, -1.0f, 1.0f);
    return powf(kContrastBase, clampedExp);
}
