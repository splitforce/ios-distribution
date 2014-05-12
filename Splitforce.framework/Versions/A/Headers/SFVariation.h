/*!
 @header    SFVariation.h
 @abstract  Splitforce Variation iOS SDK Header
 @version   1.0
 @copyright Copyright 2013 Ikura Group Limited. All rights reserved.
 */

#import <Foundation/Foundation.h>

/*!
 Splitforce iOS Variation class. Provides variation data and interface
 to report on goals caused by this variation.
 */
@interface SFVariation : NSObject


/**---------------------------------------------------------------------------------------
 * @name Getting the Variation Data
 *  ---------------------------------------------------------------------------------------
 */
/*!
 The variationData property contains a dictionary of configuration information
 which are to be used to configure the application.
 */
@property (nonatomic, strong, readonly) NSDictionary *variationData;

/**---------------------------------------------------------------------------------------
 * @name Experiment Name
 *  ---------------------------------------------------------------------------------------
 */
/*!
Get the name of the experiment that created this variation.
 */

@property (nonatomic, strong, readonly) NSString *experimentName;

/**---------------------------------------------------------------------------------------
 * @name Variation Name
 *  ---------------------------------------------------------------------------------------
 */
/*!
 Get the name of the variation that was set on Splitforce.com.
 */

@property (nonatomic, strong, readonly) NSString *name;


/**---------------------------------------------------------------------------------------
 * @name Recording Goals
 *  ---------------------------------------------------------------------------------------
 */
/*!
 When a discrete named goal has been met, call this method on the SFVariation object
 to record that goal in the backend.
 
 @param name The name of the goal to track in the Splitforce results browser.
 */
- (void)goalResultNamed:(NSString *)name;

/*!
 To track the length of time since the variation was applied, use the timedResultNamed:
 method.  This automatically tracks the length of time (in seconds) since the variation
 was applied.

  @param name The name of the goal to track in the Splitforce results browser.
 */
- (void)timedResultNamed:(NSString *)name;

/*!
 To track arbitrary lengths of time, use the timedResultNamed:withTime: method.
 Your code is repsonsible for making sure the time interval is meaningful for tracking.
 
  @param name The name of the goal to track in the Splitforce results browser.
  @param time The time value to set for this goal in the Splitforce results browser.
 */
- (void)timedResultNamed:(NSString *)name withTime:(NSTimeInterval)time;


/*!
 To track a quantified goal use the quantifiedResultNamed:quantity: method.  This is useful
 for integer quantities that happen as a result of a particular variation.

 @param name The name of the goal to track in the Splitforce results browser.
 @param quantity The number to set for this goal in the Splitforce results browser.
 */
- (void)quantifiedResultNamed:(NSString *)name quantity:(NSInteger)quantity;


/*!
 When no more variation goals are required, call variationEnded.  This will cause the variation
 to dissociate itself from its bound object and in most cases will cause the Variation to be
 dealloced.  If you retain the variation and continue to send goals, the behaviour is undefined.
 In later releases this will likely lead to an Exception being raised.
 */
- (void)variationEnded;

@end
