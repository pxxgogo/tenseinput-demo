using UnityEngine;
using System;
using System.IO;
using Academy.HoloToolkit.Unity;

#if !UNITY_EDITOR
using Windows.Networking.Sockets;
#endif

public class UDPListener : MonoBehaviour
{
#if !UNITY_EDITOR
    DatagramSocket socket;
#endif


    GUIText guiText = null;
    double timestamp = 0;
    Boolean tenseFlag = false;
    HandsManager handsManager = null;
    int smoothVal = 0;
    int smoothTag = 0;
    const int SMOOTH_THRESHOLD = 1;

    
// use this for initialization
#if !UNITY_EDITOR
    async void Start()
#else
    void Start()
#endif
    {
        guiText = GetComponentInChildren<GUIText>();
        guiText.text = "";
        timestamp = 0.0;
        handsManager = GameObject.Find("Cursor").GetComponent<HandsManager>();
#if !UNITY_EDITOR
        Debug.Log("Waiting for a connection...");

        socket = new DatagramSocket();
        socket.MessageReceived += Socket_MessageReceived;

        try
        {
            await socket.BindEndpointAsync(null, "12346");
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
            Debug.Log(SocketError.GetStatus(e.HResult).ToString());
            return;
        }

        Debug.Log("exit start");
#endif
    }

    // Update is called once per frame
    void Update()
    {
        //if (!tenseFlag)
        //{
        //    guiText.text = "0000000000000000000000";
        //}
        //else
        //{
        //    guiText.text = "1111111111111111111111";
        //}
    }

    private char[] delimiterChars = {' '};

    int GetMessage(string message)
    {
        string[] words = message.Split(delimiterChars);
        double timestamp = Convert.ToDouble(words[0]);
        if (this.timestamp < timestamp)
        {
            this.timestamp = timestamp;
            return Convert.ToInt32(words[1]);
        }
        else
            return -1;
    }

    private void operateRet(int tenseTag, string message)
    {
        if (tenseTag == 0)
        {
            if (smoothTag == 0)
            {
                smoothVal += 1;
                if (smoothVal >= SMOOTH_THRESHOLD)
                {
                    tenseFlag = false;
                    handsManager.stopDrawing();
                }
            }
            else
            {
                smoothTag = 0;
                smoothVal = 1;
            }
        }
        else if (tenseTag == 1)
        {
            if (smoothTag == 1)
            {
                smoothVal += 1;
                if (smoothVal >= SMOOTH_THRESHOLD)
                {
                    tenseFlag = true;
                    handsManager.startDrawing(); 
                }
            }
            else
            {
                smoothTag = 1;
                smoothVal = 1;
            }
        }
    }

#if !UNITY_EDITOR
    private async void Socket_MessageReceived(Windows.Networking.Sockets.DatagramSocket sender,
        Windows.Networking.Sockets.DatagramSocketMessageReceivedEventArgs args)
    {
        //Read the message that was received from the UDP echo client.
        Stream streamIn = args.GetDataStream().AsStreamForRead();
        StreamReader reader = new StreamReader(streamIn);
        string message = await reader.ReadLineAsync();
        int tenseTag = GetMessage(message);
        operateRet(tenseTag, message);
        
        
    }
#endif
}