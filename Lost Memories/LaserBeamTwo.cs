using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LaserBeamTwo : MonoBehaviour
{
    [SerializeField] private GameObject[] laserDamager;
    [SerializeField] private float maxTime;
    [SerializeField] private float minTime;
    [SerializeField] private float inActiveTime;

    private float timer;
    private bool laserActive = false;
    private void Start()
    {
        timer = inActiveTime;
    }
    // Update is called once per frame
    void Update()
    {
        ChangeState();
    }

    private void ChangeState()
    {
        if (laserActive)
        {
            return;
        }
        if (timer >= inActiveTime)
       {
            StartCoroutine(ActivateTrap());
            timer = 0;
       }
       else
       {
            timer += Time.deltaTime;
       }
    }

    private void LaserEnabler(int index)
    {
        for (int i = 0; i < laserDamager.Length; i++)
        {
            laserDamager[i].SetActive(i == index);
        }
    }

    private IEnumerator ActivateTrap()
    {
        laserActive = true;
        LaserEnabler(0);
        yield return new WaitForSeconds(UnityEngine.Random.Range(minTime, maxTime));
        LaserEnabler(1);
        yield return new WaitForSeconds(UnityEngine.Random.Range(minTime, maxTime));
        LaserEnabler(2);
        yield return new WaitForSeconds(UnityEngine.Random.Range(minTime, maxTime));
        laserDamager[2].SetActive(false);
        laserActive = false;
    }
}
