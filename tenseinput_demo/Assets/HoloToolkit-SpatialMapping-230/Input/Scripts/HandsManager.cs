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
        private List<Vector3> trackPoints = new List<Vector3>();
        private bool drawFlag = false;
        private LineManager currentLineManager = null;
        private int sample_threshold = 1;
        private int sample_num = 0;
        private Vector3 currentPosition = new Vector3(0, 0, 0);
        private bool createLineFlag = false;
        private BasicCursor basicCursor = null;
        private Vector3 offset = new Vector3(-0.04F, 0.1F, 0.05F);
        private Vector3 normal = new Vector3(0, 0, 1F);
        private List<GameObject> drawedLines = new List<GameObject>();
        private GameObject currentLine = null;


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
                    currentPosition = pos;
                    Quaternion quaternion = Quaternion.FromToRotation(normal, Camera.main.transform.forward);
                    Vector3 newPos = pos + quaternion * offset;
                    basicCursor.UpdatePosition(newPos);
                    //Debug.Log(string.Format("hand position: {0}, {1}, {2}", pos[0], pos[1], pos[2]));
                    if (drawFlag)
                    {
                        sample_num += 1;
                        if (sample_num == sample_threshold)
                        {
                            currentLineManager.FeedPoint(newPos);
                            sample_num = 0;
                        }
                    }
                }

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
                stopDrawing();
                basicCursor.lostHand();
            }
            //Debug.Log(string.Format("tracked hands: {0}", trackedHands.Count));
        }

        public void startDrawing()
        {
            if (createLineFlag || drawFlag) return;
            if (trackedHands.Count > 0)
            {
                createLineFlag = true;
            }

        }

        public void stopDrawing()
        {
            if (!drawFlag) return;
            //currentLineManager.FeedPoint(currentPosition);
            drawedLines.Add(currentLine);
            currentLine = null;
            currentLineManager = null;
            drawFlag = false;
        }

        void Update()
        {
            if (createLineFlag)
            {
                currentLine = Instantiate(linePrototype) as GameObject;
                currentLineManager = currentLine.GetComponent<LineManager>();
                Debug.Log("start drawing lines");
                //currentLineManager.FeedPoint(currentPosition);
                drawFlag = true;
                sample_num = 0;
                createLineFlag = false;
            }
            if (drawFlag)
            {
                basicCursor.SwitchToTenseMaterial();
            }
            else
            {
                basicCursor.SwitchToRelaxMaterial();
            }
        }


        void OnDestroy()
        {
            InteractionManager.SourceDetected -= InteractionManager_SourceDetected;
            InteractionManager.SourceLost -= InteractionManager_SourceLost;
        }

        public void DeleteLatestLine()
        {
            if (drawedLines.Count == 0)
                return;
            GameObject line = drawedLines[drawedLines.Count - 1];
            drawedLines.RemoveAt(drawedLines.Count - 1);
            Destroy(line, 0.1f);
        }
    }
}