using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScoreField : MonoBehaviour
{

    public enum PlayerField
    {
        P1_Field,
        P2_Field
    }
    public PlayerField MyField;

    void Start()
    {
        GameManager.TheManager.UpdateScores();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.gameObject.CompareTag("Ball"))
        {
            switch (MyField)
            {
                case PlayerField.P1_Field:
                    ++GameManager.TheManager.P1_Score;
                    break;
                case PlayerField.P2_Field:
                    ++GameManager.TheManager.P2_Score;
                    break;
            }
            SFXManager.TheSFXManager.PlaySFX(SFXManager.SoundEffectLib.Goal);
            GameManager.TheManager.UpdateScores();
            GameManager.TheManager.DespawnAll();
        }
    }
}
