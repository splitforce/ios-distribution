/*!
 @header    SFManager.h
 @abstract  Splitforce Manager iOS SDK Header
 @version   1.0
 @copyright Copyright 2013 Ikura Group Limited. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import <Splitforce/SFVariation.h>

/*!
 An Objective-C Block taking an SFVariation object as a singular parameter to apply a variation.
 */
typedef void(^SFExperimentVariationBlock)(SFVariation *variation);

/*!
 An Objective-C Block taking a BOOL as a singular parameter to indicate success or failure
 */
typedef void(^SFBooleanBlock)(BOOL success);

/*!
 An Objective-C Block with no parameters
 */
typedef void(^SFVoidBlock)(void);


/*!
 An Objective-C Block taking an NSError object as a singular parameter which may provide further details on why an error has occured.
 */
typedef void(^SFErrorBlock)(NSError *error);

/*!
 An Objective-C Block taking an NSDictionary object representing the Cohort Identifier, with experiment names as keys and variation names as values.
 */
typedef void(^SFCohortIdentifierBlock)(NSDictionary *cohortIdentifier);

/*!
 An Objective-C Block taking an NSDictionary object representing the Cohort Identifier, with experiment names as keys and variation names as values.
 The block should return nil, or a modified cohortIdentifier block which allows you to control the cohort programatically.  This should not be done
 on live experiments, but is useful when testing your code paths as you can control which paths will be executed using this method.
 */
typedef NSDictionary *(^SFWillUseCohortIdentifierBlock)(NSDictionary *cohortIdentifier);

typedef id(^SFTargetingValueBlock)(void);

/*!
 Splitforce iOS top-level class. Provides synchronisation with Splitforce backend
 and configuration of settings.
 */
@interface SFManager : NSObject

/**---------------------------------------------------------------------------------------
 * @name Settings and Configuration
 *  ---------------------------------------------------------------------------------------
 */


/*!
 The frequency of sending data to the Splitforce backend.
 */
@property (nonatomic) NSTimeInterval updateFrequency;

/**---------------------------------------------------------------------------------------
 * @name Access to the SFManager object
 *  ---------------------------------------------------------------------------------------
 */

/*!
 For convenience, this class exposes a current manager instance.
 This will be set to the first manager that is instantiated in managerWithApplicationId:applicationKey:
 or managerWithApplicationId:applicationKey:completionBlock
 It is a programming error to retrieve this manager before the first manager has been instantiated.
 Doing so will raise an Exception.
 
 @return The current SFManager
 */
+ (SFManager *)currentManager;

/*!
 Connect to the Splitforce backend.
 This method will start a connection to the Splitforce backend.  The method returns immediately
 while the connection happens in the background.  To get a callback when the manager is completely
 ready, use the managerWithApplicationId:applicationKey:completionBlock method instead.
 
 There are a number of different issues to be aware of when setting up the Splitfore connection.
 Firstly, if you apply an experiment before the connection is established, then the experimentNamed:applyVariationBlock:errorBlock
 method will block until the manager is ready (or has failed) - to prevent blocking the main thread, initialize the manager with the
 managedWithApplicationId:applicationKey:completionBlock method instead.
 
 Secondly, if an experiment is applied when the manager has failed to connect, this user will join the "default" cohort and see the
 error block variation.  See the property persistDefaultCohort for details on the behaviour in this case.
 
 Finally, if there is cached data, then that is used even if the connection fails to get new data.
 
In general - as long as the user has a functioning internet connection the first time they run the application, then
 there will be no issues.  You may decide that running your application without an active splitforce connection is
 undesirable - in which case the managerWithApplicationId:applicationKey:completionBlock method should be used to ensure
 that the initial connection has been made before proceeding further into your aplication.
 
 @param applicationId The Application Id provided by the Splitforce Web server.
 @param applicationKey The Application Key provided by the Splitforce Web server.
 @return An SFManager object connected to the specified applicationId.
 */
+ (SFManager *)managerWithApplicationId:(NSString *)applicationId applicationKey:(NSString *)applicationKey;

/*!
 Asynchronously connect to the Splitforce backend.
 This method will call the completionBlock when either the locally cached variation data
 is ready, or an updated version has been retrieved from the backend.  In case of error (e.g. first time
 usage with no network coverage it will call the completionBlock with NO.  Otherwise the completionBlock
 will be called with YES.
 
 There are a number of different issues to be aware of when setting up the Splitfore connection.
 Firstly, if you apply an experiment before the connection is established, then the experimentNamed:applyVariationBlock:errorBlock
 method will queue variation blocks until the manager is ready (or has failed) - this can lead to the user interface being updated
 in front of the user if the data completes loading after the experiment has been applied.  Waiting for the completionBlock to be called
 before applying experiments will resolve this issue.
 
 Secondly, if an experiment is applied when the manager has failed to connect, this user will join the "default" cohort and see the
 error block variation.  See the property persistDefaultCohort for details on the behaviour in this case.
 
 Finally, if there is cached data, then that is used even if the connection fails to get new data.
 
 In general - as long as the user has a functioning internet connection the first time they run the application, then
 there will be no issues.  You may decide that running your application without an active splitforce connection is
 undesirable - in which case the managerWithApplicationId:applicationKey:completionBlock method should be used to ensure
 that the initial connection has been made before proceeding further into your aplication.

 @param applicationId The Application Id provided by the Splitforce Web server.
 @param applicationKey The Application Key provided by the Splitforce Web server.
 @param completionBlock An SFBooleanBlock which will be called when the SFManager has been connected, or failed to connect.

 */
+ (void)managerWithApplicationId:(NSString *)applicationId
                  applicationKey:(NSString *)applicationKey
                 completionBlock:(SFBooleanBlock)completionBlock;

/**---------------------------------------------------------------------------------------
 * @name Class Parameters
 *  ---------------------------------------------------------------------------------------
 */

/*!
 Set the timeout for connecting to the Splitforce backend.  Note that an initial connect requires two round trips, so
 the max time any method would block or the max time for a callback to be called may be twice this value.  Note also
 that you must set this parameter before initialising the manager connection, hence this is a class method.  Changing
 the value after the manager has been established will have no effect.
 
 @param timeoutInterval The length of time in seconds for network timeouts
 */
+ (void)setTimeoutInterval:(NSTimeInterval)timeoutInterval;

/*!
 Switching on debugMode will provide more detailed logs on the console and should be switched on for all DEBUG builds.
 Debug mode also enables 'Shake to Variation' which allows you to force particular variations to be used from the UI within the app.
 Simply shake the device to get a menu of Experiments and Variations.
 */
+ (void)setDebugMode:(BOOL)debugMode;

/*!
 When using Shake to Variation, you may need to reinitialise singletons or instances that were configured with the data.
 If so then you can do that within this block.
 */
@property (nonatomic, copy) SFVoidBlock shakeToVariationDidChangeVariationBlock;

/*!
 By default, experiments are revalidated on-demand, so if the user has been placed in the default cohort due to lack of connectivity, then they will join the correct cohort soon after the data becomes available.  If you would prefer to ensure users always get the same implementation, then set this parameter to YES and we will persist the state that these users are in the default cohort, and therefore will not have experiments applied.
 */

+ (void)setPersistDefaultCohort:(BOOL)persistDefaultCohort;

/*!
 The Cohort Identifier is a dictionary with Experiment Names for keys and Variant Names for values.
 Set this block before instantiating the SFManager.  This block will then be called on the main thread
 when the cohort has been established.  The Cohort Identifier may be useful for interfacing with third
 party or bespoke Analytics services for example.
 
 Note that this block will not be called if Transient Variations is set.
 
 Also note that the default cohort is represented as an empty dictionary.
 */

+ (void)setIdentifyCohortBlock:(SFCohortIdentifierBlock)identifyCohortBlock;

/*!
 The Cohort Identifier is a dictionary with Experiment Names for keys and Variant Names for values.
 Set this block before instantiating the SFManager.  This block will then be called on the main thread
 when the cohort has been established.  The Cohort Identifier may be useful for interfacing with third
 party or bespoke Analytics services for example.
 
 Note that this block will not be called if Transient Variations is set.
 
 Also note that the default cohort is represented as an empty dictionary.

 When running in Debug mode, your cohort modification block will be called before any Shake to Variation choice is applied.  That is, Shake to Variation takes precedence over cohort modifications made in this block.
 */

+ (void)setWillUseCohortIdentifierBlock:(SFWillUseCohortIdentifierBlock)willUseCohortIdentifierBlock;

/*!
 When you define custom Segmentation/Targetting Conditions on the website, this allows you to target
 variations based on runtime data.  To pass your custom data into the targeting engine, call setCustomTargetingKey:withBlock: before intialising the splitforce library.
 The splitforce library will then execute valueBlock when required, and pass the return value into the targeting engine.
 For example, to return the user's gender from a global user object:
    [SFManager setCustomTargetingKey:@"gender" withBlock:^{
        return [UserManager.currentUser gender];
    }];
 */
+ (void)setCustomTargetingKey:(NSString *)key withBlock:(SFTargetingValueBlock)valueBlock;

/**---------------------------------------------------------------------------------------
 * @name Running Experiments
 *  ---------------------------------------------------------------------------------------
 */

/*!
 Get the data for an experiement and apply the variationBlock on the selected variation.
 The SFVariation object provided to the applyVariationBlock will contain the raw data
 in the variationData property.  The SFVariation object shall be used when goal conditions
 are met in order to accurately track the variation and result.  The - [SFVariation bindVariationToObject:] method
 is provided as a convenience an can be used in conjunction with - [SFManager variationForObject:] to
 retrieve the correct SFVariation object at a later point.

 The default block should be used to configure default settings for your user.  The default block can be called
 if there is no connection to the splitforce servers, and no cached content available for your users.

 @param experimentName The name of an experiment defined on the Splitforce Web Server.
 @param variationBlock An SFExperimentVariationBlock which will be called when there is valid data for this experiement
 @param defaultBlock An SFErrorBlock which will be called if there is no valid data for this experiement.  The NSError parameter will indicate the reason for no data.  You should configure a default version of your variation in this block.

 */
- (void)experimentNamed:(NSString *)experimentName
    applyVariationBlock:(SFExperimentVariationBlock)variationBlock
      applyDefaultBlock:(SFErrorBlock)defaultBlock;

/**---------------------------------------------------------------------------------------
 * @name Retrieving a variation for goal submission
 *  ---------------------------------------------------------------------------------------
 */

/*!
 Convenience method to retrieve the SFVariation object for the most recent application of an experiment.
 
 @param experimentName An NSString object matching the experimentName of a previously applied experiment.
 @return The SFVariation object which was most recently created for an application of this experiment

 */
- (SFVariation *)variationForExperimentNamed:(NSString *)experimentName;

/**---------------------------------------------------------------------------------------
 * @name Introspection Utilities
 *  ---------------------------------------------------------------------------------------
 */

/*!
 Get the framework version
 */
+ (NSString *)frameworkVersion;

/*!
 Get the list of known variation names for a particular experiment
 */
- (NSArray *)variationNamesForExperiment:(NSString *)experimentName;

/*!
 Get the list of known experiment names
 */
- (NSArray *)allExperimentNames;

@end
