using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectOperation : MonoBehaviour {


    private Vector3 currentPosition = new Vector3(-0.5F, 0, 0);
    private Vector3 handDistance = new Vector3(0, 0, 0);
    private Vector3 handInitDirection = new Vector3(0, 0, 0);

	// Use this for initialization
	void Start () {
		
	}


	
	// Update is called once per frame
	void Update () {
        gameObject.transform.position = currentPosition;
	}

    public void FeedHandPosition(Vector3 pos)
    {
        handDistance = currentPosition - pos;
        handInitDirection = Camera.main.transform.forward;
    }

    public void UpdatePosition(Vector3 position, bool flag=true)
    {
        if (!flag)
            currentPosition = position;
        else
        {
            Quaternion quaternion = Quaternion.FromToRotation(handInitDirection, Camera.main.transform.forward);
            currentPosition = position + quaternion * handDistance;
        }
    }
}
