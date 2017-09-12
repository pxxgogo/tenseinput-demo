// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.VR.WSA.Input;

namespace Academy.HoloToolkit.Unity
{
    /// <summary>
    /// HandsDetected determines if the hand is currently detected or not.
    /// </summary>
    public partial class HandsManager : Singleton<HandsManager>
    {
        /// <summary>
        /// HandDetected tracks the hand detected state.
        /// Returns true if the list of tracked hands is not empty.
        /// </summary>
        public bool HandDetected
        {
            get { return trackedHands.Count > 0; }
        }

        [Tooltip("Line Prototype")]
        public GameObject linePrototype;
        

        private List<uint> trackedHands = new List<uint>();
        private bool tenseFlag = false;
        private int sample_threshold = 1;
        private int sample_num = 0;
        private Vector3 currentPosition = new Vector3(0, 0, 0);
        private BasicCursor basicCursor = null;
        private Vector3 offset = new Vector3(-0.04F, 0.1F, 0.05F);
        private Vector3 normal = new Vector3(0, 0, 1F);
        private GameObject currentobject = null;
        private ObjectOperation objectOperation = null;
        private bool startTenseFlag = false;


        void Start()
        {
            basicCursor = GameObject.Find("Cursor").GetComponent<BasicCursor>();
        }

        void Awake()
        {
            InteractionManager.SourceDetected += InteractionManager_SourceDetected;
            InteractionManager.SourceLost += InteractionManager_SourceLost;
            InteractionManager.SourceUpdated += InteractionManager_SourceUpdated;
        }

        private void InteractionManager_SourceUpdated(InteractionSourceState state)
        {
            if (state.source.id == trackedHands[0])
            {
                Vector3 pos;
                if (state.properties.location.TryGetPosition(out pos))
                {
                    Quaternion quaternion = Quaternion.FromToRotation(normal, Camera.main.transform.forward);
                    Vector3 newPos = pos + quaternion * offset;
                    currentPosition = newPos;
                    basicCursor.UpdatePosition(newPos);
                    //Debug.Log(string.Format("hand position: {0}, {1}, {2}", pos[0], pos[1], pos[2]));
                    if (tenseFlag)
                    {
                        objectOperation.UpdatePosition(newPos);
                        basicCursor.UpdatePosition(newPos);
                    }                    
                }

            }
        }

        public void StartTense()
        {
            if (!tenseFlag && GazeManager.Instance.Hit && !startTenseFlag && trackedHands.Count > 0)
            {
                startTenseFlag = true;
            }
            
        }

        public void StopTense()
        {
            if(tenseFlag)
            {
                tenseFlag = false;
                basicCursor.SwitchToRelaxState();
            }
        }

        private void InteractionManager_SourceDetected(InteractionSourceState state)
        {
            // Check to see that the source is a hand.
            if (state.source.kind != InteractionSourceKind.Hand)
            {
                return;
            }
            trackedHands.Add(state.source.id);
            basicCursor.detectHand();
            //Debug.Log(string.Format("tracked hands: {0}", trackedHands.Count));
        }

        private void InteractionManager_SourceLost(InteractionSourceState state)
        {
            // Check to see that the source is a hand.
            if (state.source.kind != InteractionSourceKind.Hand)
            {
                return;
            }
            if (trackedHands.Contains(state.source.id))
            {
                trackedHands.Remove(state.source.id);
            }
            if (trackedHands.Count == 0)
            {
                basicCursor.lostHand();
            }
            //Debug.Log(string.Format("tracked hands: {0}", trackedHands.Count));
        }

        void Update()
        {
           if (startTenseFlag)
            {
                currentobject = GazeManager.Instance.HitInfo.collider.gameObject;
                objectOperation = currentobject.GetComponent<ObjectOperation>();
                objectOperation.FeedHandPosition(currentPosition);
                tenseFlag = true;
                basicCursor.SwitchToTenseState();
                startTenseFlag = false;
            }
        }


        void OnDestroy()
        {
            InteractionManager.SourceDetected -= InteractionManager_SourceDetected;
            InteractionManager.SourceLost -= InteractionManager_SourceLost;
        }

    }
}