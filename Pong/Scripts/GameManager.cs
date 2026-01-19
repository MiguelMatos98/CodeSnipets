using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameManager : MonoBehaviour
{

    public static GameManager TheManager;

    private GameObject TheBall;

    public Text P1Text;
    public Text P2Text;

    public int P1_Score;
    public int P2_Score;

    public float RespawnTimer;

    public float Interval;
    private float CurInterval;

    public GameObject BallGO;
    private bool ShouldCount = true;

    void Start()
    {
        TheBall = GameObject.Find("Ball");
        TheManager = this;
    }

    public void Update()
    {
        if (ShouldCount)
        {
            CurInterval += Time.deltaTime;
            if (CurInterval > Interval)
            {
                CurInterval = 0;
                Instantiate(BallGO, Vector3.zero, Quaternion.identity);
            }
        }
    }

    public void Despawn(GameObject BallGO)
    {
        BallGO.SetActive(false);
    }

    public IEnumerator RespawnInX(GameObject TheBallToRespawn)
    {
        yield return new WaitForSeconds(RespawnTimer);
        TheBallToRespawn.SetActive(true);
        TheBallToRespawn.GetComponent<Ball>().Respawn();
        ShouldCount = true;
        CurInterval = 0;
    }

    internal void DespawnAll()
    {
        GameObject[] BallList = GameObject.FindGameObjectsWithTag("Ball");
        for (int i = BallList.Length-1; i >= 0; i--)
        {
            Despawn(BallList[i]);
        }
        StartCoroutine(RespawnInX(BallList[0]));
        ShouldCount = false;
    }

    public void UpdateScores()
    {
        P1Text.text = P1_Score.ToString();
        P2Text.text = P2_Score.ToString();
    }
}
