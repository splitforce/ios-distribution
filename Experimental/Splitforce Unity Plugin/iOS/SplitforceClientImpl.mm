
#import "SplitforceClientImpl.h"

@interface SFManager(unityWrapperHelper)

- (SFVariation *)immediateVariationForExperimentNamed:(NSString *)experimentName;

@end

@implementation SplitforceClient

- (id)init
{
    self = [super init];
    variationForExperiment = [[NSMutableDictionary alloc] init];
    return self;
}

- (void)dealloc
{
    [variationForExperiment release];
    [manager release];
    [super dealloc];
}


- (void)setApplicationId:(NSString *)applicationId appKey:(NSString *)appKey debugMode:(BOOL)debugMode transientVariations:(BOOL)transientVariations
{
    [SFManager setSampleRate:1.0];
	[SFManager managerWithApplicationId:applicationId
                         applicationKey:appKey
                        completionBlock:^(BOOL complete){
                            manager = [SFManager.currentManager retain];
                            setupComplete = complete;
                        }];
    SFManager.currentManager.debugMode = debugMode;
    SFManager.currentManager.transientVariations = transientVariations;
}

- (BOOL)isDataAvailable
{
	return setupComplete;
}

- (NSString *)variationJSONForExperimentNamed:(NSString *)experimentName
{
	SFVariation *variation = [manager performSelector:NSSelectorFromString(@"immediateVariationForExperimentNamed:") withObject:experimentName];
	if (variation) variationForExperiment[experimentName] = variation;
    else return nil;

	NSDictionary *data = variation.variationData;
	NSData *jsonData = [NSJSONSerialization dataWithJSONObject:data options:0 error:nil];
    return [[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding] autorelease];
}

- (void) postGoal:(NSString *)goalName forExperimentNamed:(NSString *)experimentName
{
    SFVariation *variation = variationForExperiment[experimentName];
    if (variation == nil) return;

    [variation goalResultNamed:goalName];
}

- (void) postTime:(NSString *)goalName forExperimentNamed:(NSString *)experimentName time:(double)time
{
    SFVariation *variation = variationForExperiment[experimentName];
    if (variation == nil) return;

    [variation timedResultNamed:goalName withTime:time];
}

- (void) postElapsedTime:(NSString *)goalName forExperimentNamed:(NSString *)experimentName;
{
    SFVariation *variation = variationForExperiment[experimentName];
    if (variation == nil) return;

    [variation timedResultNamed:goalName];
}

- (void) postQuantityGoal:(NSString *)goalName forExperimentNamed:(NSString *)experimentName quantity:(double)quantity
{
    SFVariation *variation = variationForExperiment[experimentName];
    if (variation == nil) return;

    [variation quantifiedResultNamed:goalName quantity:quantity];
}

- (void) finishExperimentNamed:(NSString *)experimentName
{
    [variationForExperiment removeObjectForKey:experimentName];
}

@end

static SplitforceClient *clientObject = nil;

// Converts C style string to NSString
NSString* CreateNSString (const char* string)
{
	if (string)
		return [NSString stringWithUTF8String: string];
	else
		return [NSString stringWithUTF8String: ""];
}

// Helper method to create C string copy
char* MakeStringCopy (const char* string)
{
	if (string == NULL)
		return NULL;

	char* res = (char*)malloc(strlen(string) + 1);
	strcpy(res, string);
	return res;
}

// When native code plugin is implemented in .mm / .cpp file, then functions
// should be surrounded with extern "C" block to conform C function naming rules
extern "C" {

	void _SetApplicationIdAndKey (char* appId, char* appKey, bool transientVariations, bool debugMode)
	{
		if (clientObject == nil)
			clientObject = [[SplitforceClient alloc] init];

		[clientObject setApplicationId:CreateNSString(appId) appKey:CreateNSString(appKey) debugMode:debugMode transientVariations:transientVariations];
	}

	bool _GetDataAvailableStatus ()
	{
		return ([clientObject isDataAvailable]);
	}

	char* _GetVariationJSONForExperiment (char* experimentName)
	{
		return MakeStringCopy([[clientObject variationJSONForExperimentNamed:CreateNSString(experimentName)] UTF8String]);
	}

	void _PostGoalForExperiment (char* experimentName, char* goalName)
	{
		[clientObject postGoal:CreateNSString(goalName) forExperimentNamed:CreateNSString(experimentName)];
	}

	void _PostTimeForExperiment (char* experimentName, char* goalName, float time)
	{
		[clientObject postTime:CreateNSString(goalName) forExperimentNamed:CreateNSString(experimentName) time:time];
	}

	void _PostElapsedTimeForExperiment (char* experimentName, char* goalName)
	{
		[clientObject postElapsedTime:CreateNSString(goalName) forExperimentNamed:CreateNSString(experimentName)];
	}

	void _PostQuantityForExperiment (char* experimentName, char* goalName, float quantity)
	{
		[clientObject postQuantityGoal:CreateNSString(goalName) forExperimentNamed:CreateNSString(experimentName) quantity:quantity];
	}

    void _FinishExperiment (char* experimentName)
	{
		[clientObject finishExperimentNamed:CreateNSString(experimentName)];
	}

}

