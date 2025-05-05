using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class SteamTrap : MonoBehaviour
{

    // Start is called before the first frame update



    [SerializeField] private GameObject steamDamager;
    private bool isActive = true;
    [SerializeField] private float maxTime;
    [SerializeField] private float minTime;
    [SerializeField] private float activeTime;
    private float timer;


    // Update is called once per frame
    void Update()
    {

        ChangeState();
    }

    private void ChangeState()
    {

        if (timer <= 0)
        {
            if (isActive)
            {
                DesactivateTrap();
                timer = Random.Range(minTime, maxTime);
            }
            else
            {
                ActivateTrap();
                timer = activeTime;
            }
            isActive = !isActive;
        }
        else
        {
            timer -= Time.deltaTime;

        }
    }

    private void ActivateTrap()
    {
        steamDamager.SetActive(true);
    }

    private void DesactivateTrap()
    {
        steamDamager.SetActive(false);
    }


}
