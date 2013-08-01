#import <Foundation/Foundation.h>
#import <Splitforce/SFKit.h>


@interface SplitforceClient : NSObject
{
	// Keep track of variations for experiments
	NSMutableDictionary *variationForExperiment;
	SFManager *manager;
	__block BOOL setupComplete;
}

- (void) setApplicationId:(NSString *)applicationId appKey:(NSString *)appKey debugMode:(BOOL)debugMode transientVariations:(BOOL)transientVariations;
- (BOOL) isDataAvailable;
- (NSString *) variationJSONForExperimentNamed:(NSString *)experimentName;
- (void) postGoal:(NSString *)goalName forExperimentNamed:(NSString *)experimentName;
- (void) postTime:(NSString *)goalName forExperimentNamed:(NSString *)experimentName time:(double)time;
- (void) postElapsedTime:(NSString *)goalName forExperimentNamed:(NSString *)experimentName;
- (void) postQuantityGoal:(NSString *)goalName forExperimentNamed:(NSString *)experimentName quantity:(double)quantity;
- (void) finishExperimentNamed:(NSString *)experimentName;

@end

