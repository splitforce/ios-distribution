/*
The Unity Wrapper provides an interface to the Splitforce framework which is more appropriate for frame-based
game engines.

Simple usage instructions are:
1. Drag the Splitforce.framework to you XCode project, and add the CoreData.framework
	dependency in your Build Phases -> Link With Libraries step
2. Drag the contents of the Splitforce Unity Plugin folder into Unity's Assets->Plugins folder (create it if necessary)

3. At the earliest point in your game's lifecycle, make a call to Splitforce.SetApplicationIdAndKey with the AppId and AppKey
	provided by the Splitforce web site.
4. Poll for Splitforce.GetDataAvailableStatus() until it is true, before applying any experiments

5. When you're ready to configure an object with data from a variation, you can grab the JSON representation
	with Splitfoce.GetVariationJSONForExperiment(name) - we've included JSONObject and Nullable plugins to aid
	with converting the variation into an object - e.g. new JSONObject(Splitforce.GetVariationJSONForExperiment("experimentName"))

6. When a goal conversion or other trackable event occurs, you can call the Splitforce library with:
	Splitforce.PostGoalForExperiment(experimentName, goalName)
	and also PostTimeForExperiment, PostElapsedTimeForExperiment, PostQuantityForExperiment

7. Finally, when you no longer need the variation data and will have no further goals to post, you can call
	Splitfoce.FinishExperiment(experimentName)
*/

using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class Splitforce {

	/* Interface to native implementation */
	
	[DllImport ("__Internal")]
	private static extern void _SetApplicationIdAndKey (string appId, string appKey, bool transientVariations, bool debugMode);

	[DllImport ("__Internal")]
	private static extern bool _GetDataAvailableStatus ();

	[DllImport ("__Internal")]
	private static extern string _GetVariationJSONForExperiment (string experimentName);
		
	[DllImport ("__Internal")]
	private static extern void _PostGoalForExperiment (string experimentName, string goalName);
	
	[DllImport ("__Internal")]
	private static extern void _PostTimeForExperiment (string experimentName, string goalName, float time);

	[DllImport ("__Internal")]
	private static extern void _PostElapsedTimeForExperiment (string experimentName, string goalName);

	[DllImport ("__Internal")]
	private static extern void _PostQuantityForExperiment (string experimentName, string goalName, float quantity);

	[DllImport ("__Internal")]
	private static extern void _FinishExperiment (string experimentName);

	/* Public interface for use inside C# / JS code */
	
	// Sets the Application Id and Key to connect to Splitforce
	public static void SetApplicationIdAndKey(string appId, string appKey, bool transientVariations, bool debugMode)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_SetApplicationIdAndKey(appId, appKey, transientVariations, debugMode);
	}

	// Return the status of data availability, false if we are still loading
	// True if we have locally cached data available, successfully downloaded, or we have positively failed to connect
	public static bool GetDataAvailableStatus ()
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
		{
			return _GetDataAvailableStatus();
		}
		// Return mockup values for code running inside Editor
		else
		{
			return true;
		}
	}

	
	// Return variation data as UTF8 encoded JSON String
	public static string GetVariationJSONForExperiment(string experimentName)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
		{
			return _GetVariationJSONForExperiment(experimentName);
		}
		// Return mockup values for code running inside Editor
		else
		{
			return "";
		}
	}
	
	// Post a basic conversion goal
	public static void PostGoalForExperiment (string experimentName, string goalName)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_PostGoalForExperiment(experimentName, goalName);
	}

	// Post a timed goal with time measured in seconds within your unity code
	public static void PostTimeForExperiment (string experimentName, string goalName, float time)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_PostTimeForExperiment(experimentName, goalName, time);
	}

	// Post a timed goal with the time elapsed since the variation data was passed to your unity code
	public static void PostElapsedTimeForExperiment (string experimentName, string goalName)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_PostElapsedTimeForExperiment(experimentName, goalName);
	}

	// Post a quantified goal
	public static void PostGoalForExperiment (string experimentName, string goalName, float quantity)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_PostQuantityForExperiment(experimentName, goalName, quantity);
	}
	
	// Finish an experiment and send no more analytics
	public static void FinishExperiment (string experimentName)
	{
		// Call plugin only when running on real device
		if (Application.platform != RuntimePlatform.OSXEditor)
			_FinishExperiment(experimentName);
	}
}
