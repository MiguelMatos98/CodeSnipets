using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ship : MonoBehaviour
{
    public float turnRate;
    public float accelleration;

    public bool isAccellerating = false;

    public Sprite normal;
    public Sprite boost;

    private Rigidbody2D rigidBody;
    private SpriteRenderer spriteRenderer;
    private Collider2D boxCollider;

	public static Vector2 halfScreen;

	public Camera mainCamera;

	[SerializeField] private Vector3 newPosition;

	[Header("Bullet Category")]
	public GameObject bullet;

	public float bulletSpeed;

	public float bulletLifeTime;

	private bool lockInputs;

	public float respawnTimer;

	[Header("Blink Category")]
	public bool blinking;

	private float currentBlinkInterval;

	public float blinkInterval;
	
	public float blinkDuration;

	[Header("Fragments Category")]
	public int fragmentCount;

	public float fragmentSpeed;

	public GameObject fragmentObject;

	void Awake()
    {
        rigidBody = GetComponent<Rigidbody2D>();
        spriteRenderer = GetComponent<SpriteRenderer>();
        boxCollider = GetComponent<BoxCollider2D>();

        halfScreen = new Vector2(mainCamera.aspect * mainCamera.orthographicSize, mainCamera.orthographicSize);
	}
    void Update()
    {
		if (blinking)
		{
			currentBlinkInterval += Time.deltaTime;
			if (currentBlinkInterval > blinkInterval)
			{
				currentBlinkInterval = 0;
				spriteRenderer.enabled = !spriteRenderer.enabled;
			}
		}

		if (lockInputs)
		{
			spriteRenderer.sprite = normal;
			return;
		}

		if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow))
        {
            transform.Rotate(0,0, turnRate * Time.deltaTime);
		}
		else if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow))
		{
			transform.Rotate(0, 0, -turnRate * Time.deltaTime);
		}

        isAccellerating = (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow));
		spriteRenderer.sprite = isAccellerating? boost : normal;

        if (isAccellerating)
			rigidBody.AddForce(transform.up * accelleration);

		Vector2 relitiveCameraDist = transform.position - mainCamera.transform.position;

		newPosition = transform.position;

		if (relitiveCameraDist.x <= -halfScreen.x)
		{
			newPosition.x += halfScreen.x * 2;
		}
		else if (relitiveCameraDist.x >= halfScreen.x)
		{
			newPosition.x -= halfScreen.x * 2;
		}

		if (relitiveCameraDist.y <= -halfScreen.y)
		{
			newPosition.y += halfScreen.y * 2;
		}
		else if (relitiveCameraDist.y >= halfScreen.y)
		{
			newPosition.y -= halfScreen.y * 2;
		}

		transform.position = newPosition;

		if (Input.GetKeyDown(KeyCode.Space))
		{
			GameObject newBullet = Instantiate(bullet, transform.position, Quaternion.identity);

			newBullet.GetComponent<Rigidbody2D>().linearVelocity = transform.up * bulletSpeed;

			Destroy(newBullet, bulletLifeTime);
		}
	}

	private void OnTriggerEnter2D(Collider2D collision)
	{
		if (collision.CompareTag("Asteroid") || collision.CompareTag("EnemyBullet"))
		{
			if(collision.name.Contains("Bullet"))
				collision.gameObject.SetActive(false);

			spriteRenderer.enabled = false;
			boxCollider.enabled = false;
			lockInputs = true;
			StartCoroutine(Respawn());

			for (int i = 0; i < fragmentCount; i++)
			{
				float currentSpeed = (i < fragmentCount /2) ? fragmentSpeed : fragmentSpeed * 2;

				GameObject newFragment = Instantiate(fragmentObject, transform.position, Quaternion.identity);
				newFragment.GetComponent<Rigidbody2D>().linearVelocity = Random.insideUnitCircle.normalized * currentSpeed;
				Destroy(newFragment, 2.5f);
			}
		}
	}

	private IEnumerator Respawn()
	{
		yield return new WaitForSeconds(respawnTimer);
		lockInputs = false;
		spriteRenderer.enabled = true;
		transform.position = Vector2.zero;
		rigidBody.linearVelocity = Vector2.zero;
		transform.rotation = Quaternion.identity;
		blinking = true;
		StartCoroutine(EndBlinking());
	}

	private IEnumerator EndBlinking()
	{
		yield return new WaitForSeconds(blinkDuration);
		boxCollider.enabled = true;
		spriteRenderer.enabled = true;
		blinking = false;
	}
}
