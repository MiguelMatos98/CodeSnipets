using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

public class HealthPickUp : MonoBehaviour
{
    [SerializeField] private int amount;
    [SerializeField] private ParticleSystem healthparticles;
    [SerializeField] private BoxCollider2D hitbox;
    [SerializeField] private SpriteRenderer healthsprite;

    private bool triggered;
    public void GiveHealth()
    {
        if (PlayerController.Instance.GetComponent<HealthSystem>().currentHealth != 100)
        {
            PlayerController.Instance.GetComponent<HealthSystem>().RestoreHeal(amount);
            CheckpointManager.Instance.RemoveFromCollectables(gameObject);
            triggered = true;
            
        }


    }


    void FixedUpdate()
    {
        if (triggered)
        {
            healthparticles.Play();
            hitbox.enabled = false;
            healthsprite.enabled = false;
            triggered = false;
        }




    }





    
}
