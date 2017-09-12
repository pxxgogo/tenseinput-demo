using UnityEngine;
using UnityEngine.VR.WSA;
using Academy.HoloToolkit.Unity;
using System.Collections.Generic;

public class ObjectManager : MonoBehaviour
{

    private BasicCursor basicCursor;
    public List<GameObject> objectPrototypes = new List<GameObject>();
    private bool createFlag = false;


    // Use this for initialization
    void Start()
    {
        basicCursor = GameObject.Find("Cursor").GetComponent<BasicCursor>();
    }

    public void CreateObject()
    {
        createFlag = true;
    }

    public void DestroyObject()
    {
        Destroy(GazeManager.Instance.HitInfo.collider.gameObject, 0.1F);
    }

    // Update is called once per frame
    void Update()
    {
        if (createFlag)
        {
            createFlag = false;
            Vector3 position = basicCursor.GetPosition();
            Quaternion rotation = Quaternion.LookRotation(Vector3.forward, Vector3.up);
            Debug.LogFormat("{0}, {1}", position, rotation);
            int index = Random.Range(0, objectPrototypes.Count);
            GameObject gameObject = Instantiate(objectPrototypes[index]) as GameObject;
            gameObject.GetComponent<ObjectOperation>().UpdatePosition(position, false);

        }
    }
}
