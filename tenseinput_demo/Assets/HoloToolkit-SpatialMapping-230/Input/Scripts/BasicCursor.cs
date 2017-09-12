// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using UnityEngine;

namespace Academy.HoloToolkit.Unity
{
    /// <summary>
    /// 1. Decides when to show the cursor.
    /// 2. Positions the cursor at the gazed location.
    /// 3. Rotates the cursor to match hologram normals.
    /// </summary>
    public class BasicCursor : MonoBehaviour
    {
        [Tooltip("Distance, in meters, to offset the cursor from the collision point.")]
        public float DistanceFromCollision = 0.01f;

        [Tooltip("material rendered when relax")]
        public Material relaxMaterial;

        [Tooltip("material rendered when tense")] 
        public Material tenseMaterial;


        private Quaternion cursorDefaultRotation;

        private MeshRenderer meshRenderer;

        private Vector3 currentPosition;

        private bool handTracking = false;

        private bool tenseFlag = false;

        void Awake()
        {
            //if (GazeManager.Instance == null)
            //{
            //    Debug.Log("Must have a GazeManager somewhere in the scene.");
            //    return;
            //}

            //if ((GazeManager.Instance.RaycastLayerMask & this.gameObject.layer) == 0)
            //{
            //    Debug.LogError("The cursor has a layer that is checked in the GazeManager's Raycast Layer Mask.  Change the cursor layer (e.g.: to Ignore Raycast) or uncheck the layer in GazeManager: " +
            //        LayerMask.LayerToName(this.gameObject.layer));
            //}

            meshRenderer = this.gameObject.GetComponent<MeshRenderer>();


            //if (meshRenderer == null)
            //{
            //    Debug.Log("This script requires that your cursor asset has a MeshRenderer component on it.");
            //    return;
            //}

            //// Hide the Cursor to begin with.
            //meshRenderer.enabled = false;

            //// Cache the cursor default rotation so the cursor can be rotated with respect to the original orientation.
            //cursorDefaultRotation = this.gameObject.transform.rotation;
        }

        public void UpdatePosition(Vector3 pos)
        {
            currentPosition = pos;
        }

        public void detectHand()
        {
            handTracking = true;
        }

        public void lostHand()
        {
            handTracking = false;
        }

        public void SwitchToRelaxMaterial()
        {
            if(tenseFlag)
            {
                meshRenderer.material = relaxMaterial;
                tenseFlag = false;
            }
        }

        public void SwitchToTenseMaterial()
        {
            if (!tenseFlag)
            {
                meshRenderer.material = tenseMaterial;
                tenseFlag = true;
            }
        }

        void LateUpdate()
        {
            if (!handTracking || meshRenderer == null)
            {
                meshRenderer.enabled = false;
                return;
            }
            meshRenderer.enabled = true;
            //Debug.Log("work");
            // Place the cursor at the calculated position.
            this.gameObject.transform.position = currentPosition;
            // Reorient the cursor to match the hit object normal.
            this.gameObject.transform.up = Camera.main.transform.forward;
        }
    }
}