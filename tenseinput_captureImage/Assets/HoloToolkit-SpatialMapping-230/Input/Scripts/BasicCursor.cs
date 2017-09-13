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

        [Tooltip("material rendered when undectected")]
        public Material undetectedMaterial;

        [Tooltip("material rendered when detected")]
        public Material detectedMaterial;

        [Tooltip("material rendered when tense")]
        public Material tenseMaterial;


        private Quaternion cursorDefaultRotation;

        private MeshRenderer meshRenderer;

        private Vector3 keyPosition;

        private bool handTracking = false;

        private bool tenseFlag = false;

        public Vector3 GetPosition()
        {
            return this.gameObject.transform.position;
        }

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
            meshRenderer.enabled = true;


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
            keyPosition = pos;
        }

        public void detectHand()
        {
            handTracking = true;
        }

        public void lostHand()
        {
            handTracking = false;
        }

        public void SwitchToRelaxState()
        {
            if (tenseFlag)
            {
                tenseFlag = false;
            }
        }

        public void SwitchToTenseState()
        {
            if (!tenseFlag)
            {
                tenseFlag = true;
            }
        }

        void LateUpdate()
        {
            if (GazeManager.Instance == null || meshRenderer == null)
            {
                return;
            }

            if (tenseFlag)
            {
                meshRenderer.enabled = false;
                return;
            }
            else
            {
                meshRenderer.enabled = true;
            }
            if (GazeManager.Instance.Hit)
            {
                this.gameObject.transform.position = GazeManager.Instance.Position + GazeManager.Instance.Normal * DistanceFromCollision;

                // Reorient the cursor to match the hit object normal.
                meshRenderer.material = detectedMaterial;
                this.gameObject.transform.up = GazeManager.Instance.Normal;
                this.gameObject.transform.rotation *= cursorDefaultRotation;
            }
            else
            {
                meshRenderer.material = undetectedMaterial;
                this.gameObject.transform.position = Camera.main.transform.position + Camera.main.transform.forward * 1.0F;
                this.gameObject.transform.up = Camera.main.transform.forward;
                //Debug.Log("Undetected!");
                //Debug.LogFormat("({0}, {1})", Camera.main.transform.position, this.gameObject.transform.position);
            }

            // Place the cursor at the calculated position.

        }
    }
}