using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UFO : MonoBehaviour
{
    public float shootInterval;
    public float shootSpeed;
    public float ufoSpeed;
	private float currentShootInterval;

	private Vector2 direction;

	private Rigidbody2D rigidBody;

	public GameObject ufoBullet;

	private GameObject player;

	void Start()
    {
		player = GameObject.FindGameObjectWithTag("Player");

		rigidBody = GetComponent<Rigidbody2D>();

		direction.x = Random.Range(-1.0f, 1.0f);
		direction.y = Random.Range(-1.0f, 1.0f);

		rigidBody.linearVelocity = direction.normalized * ufoSpeed;
	}

	void Update()
    {
        currentShootInterval += Time.deltaTime;
        if (currentShootInterval >= shootInterval)
        {
            currentShootInterval = 0f;

			Vector2 shootDirection = player.transform.position - transform.position;

			GameObject newBullet = Instantiate(ufoBullet, transform.position, Quaternion.identity);
			newBullet.GetComponent<Rigidbody2D>().linearVelocity = shootDirection.normalized * shootSpeed;
			Destroy(newBullet, 10.0f);
		}
	}

	private void OnTriggerEnter2D(Collider2D collision)
	{
		if(collision.CompareTag("Bullet"))
		{
			ScoreManager.instance.UpdateScore(ScoreManager.ScoreType.UFO);
			gameObject.SetActive(false);
			collision.gameObject.SetActive(false);
			Destroy(gameObject);
		}
	}
}
