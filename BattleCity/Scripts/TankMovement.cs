using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static RocketScript;

public class TankMovement : MonoBehaviour
{
    private float horizontalInput;

	private float verticalInput;

    private Rigidbody2D rigidBody;

    private Animator animator;

    public float speed = 5f;

    public GameObject bullet;

    public float bulletSpeed = 10f;

	public RocketScript.PlayerID playerID;

    public int health;

    private string inputString;

    private KeyCode inputKey;

    public float shootInterval;
    private float currentShootInterval;
	private bool canShoot;

	void Awake()
	{
        canShoot = true;

		rigidBody = GetComponent<Rigidbody2D>();

        animator = GetComponent<Animator>();

        if(this.playerID == RocketScript.PlayerID.One)
        {
            inputString = "_1";
			inputKey = KeyCode.Space;
		}
		else if (this.playerID == RocketScript.PlayerID.Two)
		{
            inputString = "_2";
            inputKey = KeyCode.Return;
		}
	}

	void Update()
    {
        horizontalInput = Input.GetAxisRaw("Horizontal" + inputString);
        verticalInput = Input.GetAxisRaw("Vertical" + inputString);

        animator.SetBool("IsMoving", horizontalInput != 0 || verticalInput != 0);
	    
        if (horizontalInput != 0 || verticalInput != 0)
        {
            Vector3 movement = new Vector3(horizontalInput, verticalInput, 0).normalized;
			Vector3 up = Vector3.Cross(Vector3.forward, movement);
			Quaternion targetRotation = Quaternion.LookRotation(Vector3.forward, up);
            transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, Time.deltaTime * 10f);
        }

        rigidBody.linearVelocity = new Vector3(horizontalInput, verticalInput, 0) * speed;

        currentShootInterval += Time.deltaTime;

		canShoot = (currentShootInterval > shootInterval);

		if (Input.GetKeyDown(inputKey) && canShoot)
		{
            currentShootInterval = 0.0f;
            
            GameObject newBullet = Instantiate(bullet, transform.GetChild(0).transform.position, transform.rotation);
            newBullet.GetComponent<RocketScript>().playerID = this.playerID;
            newBullet.GetComponent<SpriteRenderer>().color = GetComponent<SpriteRenderer>().color;
            newBullet.GetComponent<Rigidbody2D>().linearVelocity = transform.right * bulletSpeed;
        }
	}
}
