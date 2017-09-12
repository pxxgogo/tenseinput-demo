// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using UnityEngine;
using UnityEngine.VR.WSA.Input;

namespace Academy.HoloToolkit.Unity
{
    /// <summary>
    /// GestureManager creates a gesture recognizer and signs up for a tap gesture.
    /// When a tap gesture is detected, GestureManager uses GazeManager to find the game object.
    /// GestureManager then sends a message to that game object.
    /// </summary>
    public partial class GestureManager : Singleton<GestureManager>
    {
        /// <summary>
        /// To select even when a hologram is not being gazed at,
        /// set the override focused object.
        /// If its null, then the gazed at object will be selected.
        /// </summary>
        public GameObject OverrideFocusedObject
        {
            get; set;
        }

        /// <summary>
        /// Gets the currently focused object, or null if none.
        /// </summary>
        public GameObject FocusedObject
        {
            get { return focusedObject; }
        }

        private GestureRecognizer gestureRecognizer;
        private GameObject focusedObject;
        private HandsManager handsManager = null;
        private bool drawFlag = false;


        void Start()
        {
            // Create a new GestureRecognizer. Sign up for tapped events.
            gestureRecognizer = new GestureRecognizer();
            gestureRecognizer.SetRecognizableGestures(GestureSettings.Tap | GestureSettings.DoubleTap);

            gestureRecognizer.TappedEvent += GestureRecognizer_TappedEvent;
            handsManager = GameObject.Find("Cursor").GetComponent<HandsManager>();
            // Start looking for gestures.
            gestureRecognizer.StartCapturingGestures();

        }

        private void GestureRecognizer_TappedEvent(InteractionSourceKind source, int tapCount, Ray headRay)
        {
            if (tapCount == 2)
            {
                handsManager.DeleteLatestLine();
            }
        }

        void LateUpdate()
        {
        }

        void OnDestroy()
        {
            gestureRecognizer.StopCapturingGestures();
            gestureRecognizer.TappedEvent -= GestureRecognizer_TappedEvent;
        }
    }
}