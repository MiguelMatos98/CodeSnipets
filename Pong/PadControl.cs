using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PadControl : MonoBehaviour
{

    public float VertSpeed;

    private Rigidbody2D MyRigid;

    public enum Player
    {
        P1,
        P2
    }

    public Player MyPlayer;
    private string PlayerString;
    public float Y_Lim;

    void Start()
    {
        MyRigid = GetComponent<Rigidbody2D>();

        switch (MyPlayer)
        {
            case Player.P1:
                PlayerString = "_P1";
                break;
            case Player.P2:
                PlayerString = "_P2";
                break;
            default:
                break;
        }
    }

    void Update()
    {
        
        MyRigid.velocity = new Vector2(0, Input.GetAxisRaw("Vertical"+ PlayerString) * VertSpeed);
    }

    private void LateUpdate()
    {
        if (transform.position.y > Y_Lim)
            transform.position = new Vector3(transform.position.x, Y_Lim, transform.position.z);

        if(transform.position.y < -Y_Lim)
            transform.position = new Vector3(transform.position.x, -Y_Lim, transform.position.z);
    }


}
