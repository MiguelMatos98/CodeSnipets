using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ball : MonoBehaviour
{

    public enum StartDir
    {
        UpRight,
        UpLeft,
        DownRight,
        DownLeft
    }
    private StartDir MyStartingDir;

    private Rigidbody2D MyRigid;
    
    public float BallSpeed;

    private Vector3 OriginPos;

    public float BallSpeedIncrements = 0.005f;
    public float BallSpeedAdjustment = 1;

    public void Awake()
    {
        OriginPos = transform.position;
    }

    void Start()
    {
        MyRigid = GetComponent<Rigidbody2D>();

        Respawn();
    }

    public void Respawn()
    {
        BallSpeedAdjustment = 1;
        transform.position = OriginPos;
        SFXManager.TheSFXManager.PlaySFX(SFXManager.SoundEffectLib.Respawn);

        int Rand = UnityEngine.Random.Range(0, 4);
        MyStartingDir = (StartDir)Rand;

        switch (MyStartingDir)
        {
            case StartDir.UpRight:
                MyRigid.velocity = new Vector2(1, 1) * BallSpeed;
                break;
            case StartDir.UpLeft:
                MyRigid.velocity = new Vector2(-1, 1) * BallSpeed;
                break;
            case StartDir.DownRight:
                MyRigid.velocity = new Vector2(1, -1) * BallSpeed;
                break;
            case StartDir.DownLeft:
                MyRigid.velocity = new Vector2(-1, -1) * BallSpeed;
                break;
            default:
                break;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Wall"))
        {
            BallSpeedAdjustment += BallSpeedIncrements;
            MyRigid.velocity = new Vector2(MyRigid.velocity.x, -MyRigid.velocity.y) * BallSpeedAdjustment;
            SFXManager.TheSFXManager.PlaySFX(SFXManager.SoundEffectLib.Bounce);
        }
        else if (collision.CompareTag("Pad"))
        {
            BallSpeedAdjustment += BallSpeedIncrements;
            MyRigid.velocity = new Vector2(-MyRigid.velocity.x, MyRigid.velocity.y) * BallSpeedAdjustment;
            SFXManager.TheSFXManager.PlaySFX(SFXManager.SoundEffectLib.Bounce);
        }
    }
}
