using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.Design;
using UnityEngine;

public class LaserBeamTrap : MonoBehaviour
{
    [SerializeField] private GameObject laserDamagerOne;
    [SerializeField] private GameObject laserDamagerTwo;
    [SerializeField] private GameObject laserDamagerThree;
    [SerializeField] private float maxTime;
    [SerializeField] private float minTime;
    [SerializeField] private float activeTime;
    private float timer;

    private void Start()
    {
            timer = Random.Range(minTime, maxTime);
    }
    // Update is called once per frame
    void Update()
    {

        ChangeState();
    }

    private void ChangeState()
    {
        

        if (timer <= 0)
        {
            if (laserDamagerOne.activeInHierarchy == false)
            {
                ActivateTrap(laserDamagerOne);
                timer = Random.Range(minTime, maxTime);
                
            }
            else if (laserDamagerTwo.activeInHierarchy == false && laserDamagerOne.activeInHierarchy == true)
            {
                DesactivateTrap(laserDamagerOne);
                ActivateTrap(laserDamagerTwo);
            }
            else
            {
                Debug.Log("Bugging");
            }


        }
        else
        {
            timer -= Time.deltaTime;

        }
        
    }

    private void ActivateTrap(GameObject laserObject)
    {
        laserObject.SetActive(true);
    }

    private void DesactivateTrap(GameObject laserObject)
    {
        laserObject.SetActive(false);
    }

}
