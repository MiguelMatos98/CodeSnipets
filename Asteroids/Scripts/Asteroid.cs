using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Asteroid : MonoBehaviour
{
    public int health;

    public float turnRate;

    public float speed;

    public float shrinkRate;

    private Vector2 direction;

    private Rigidbody2D rigidBody;

	public GameObject asteroid;

	public void InitializeAsteroidComponents()
	{
		rigidBody = GetComponent<Rigidbody2D>();
	}

	void Start()
    {
		direction.x = Random.Range(-1.0f, 1.0f);
		direction.y = Random.Range(-1.0f, 1.0f);

        rigidBody.linearVelocity = direction.normalized * speed;
	}

    void Update()
    {
		transform.Rotate(0, 0, turnRate * Time.deltaTime);
	}

	private void OnTriggerEnter2D(Collider2D collision)
	{
		if(collision.CompareTag("Bullet"))
		{
			ScoreManager.instance.UpdateScore(ScoreManager.ScoreType.Asteroid);

			health--;

			if (health > 0)
			{
				// Spawn 2 things

				GameObject newAsteroid1 = AsteroidManager.instance.SpawnAsteroid(asteroid, transform.position, Quaternion.identity);
				newAsteroid1.GetComponent<Asteroid>().health = health;
				newAsteroid1.transform.localScale = transform.localScale * shrinkRate;

				GameObject newAsteroid2 = AsteroidManager.instance.SpawnAsteroid(asteroid, transform.position, Quaternion.identity);
				newAsteroid2.GetComponent<Asteroid>().health = health;
				newAsteroid2.transform.localScale = transform.localScale * shrinkRate;
			}
			gameObject.SetActive(false);
			collision.gameObject.SetActive(false);
			Destroy(gameObject);
		}
	}
}
