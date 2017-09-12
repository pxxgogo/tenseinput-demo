using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LineManager : MonoBehaviour {

    private LineRenderer lineRenderer = null;
    private List<Vector3> points = new List<Vector3>();
    private bool updateFlag = false;
	// Use this for initialization
    void Start () {
        lineRenderer = gameObject.GetComponent<LineRenderer>();
        //lineRenderer.endColor = new Color(1, 0, 0);
        //lineRenderer.startColor = new Color(0, 1, 0);
        //Material newMat = Resources.Load("Sun", typeof(Material)) as Material;
        //Debug.Log(newMat);
        //lineRenderer.material = newMat;
        //float alpha = 1.0f;
        //Gradient gradient = new Gradient();
        //gradient.SetKeys(
        //    new GradientColorKey[] { new GradientColorKey(Color.green, 0.0f), new GradientColorKey(Color.red, 1.0f) },
        //    new GradientAlphaKey[] { new GradientAlphaKey(alpha, 0.0f), new GradientAlphaKey(alpha, 1.0f) }
        //    );
        //lineRenderer.colorGradient = gradient;
    }

    public void FeedPoint(Vector3 point)
    {
        if (points.Count == 0 || points[points.Count - 1] != point)
        {
            points.Add(point);
            updateFlag = true;
        }

    }

    // Update is called once per frame
    void LateUpdate () {
        if (updateFlag)
        {
            updateFlag = false;
            lineRenderer.positionCount = points.Count;
            lineRenderer.numCapVertices = 2;
            //lineRenderer.numCornerVertices = points.Count;
            lineRenderer.SetPositions(points.ToArray());
        }

    }
}
