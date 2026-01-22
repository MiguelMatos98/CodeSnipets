using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SocialPlatforms;

public class AsteroidManager : MonoBehaviour
{
    public List<Sprite> asteroidSprites = new List<Sprite>();

    public static AsteroidManager instance;

    public float asteroidSpawnInterval;

    private float currentAsteroidSpawnTime;

	public GameObject asteroid;

	[SerializeField] private float asteroidSafeRegion = 5.5f;

	public int maxAsteroids;

	public float spawnTime = 10;
	private float currentSpawnTime;

	public GameObject ufo;

	void Start()
    {
        instance = this;
	}

    void Update()
    {
		if (maxAsteroids > 0)
		{
			currentAsteroidSpawnTime += Time.deltaTime;

			if (currentAsteroidSpawnTime >= asteroidSpawnInterval)
			{
				currentAsteroidSpawnTime = 0;

				maxAsteroids--;

				SpawnAsteroid(asteroid, PickLocation(asteroidSafeRegion), Quaternion.identity);
			}
		}

		currentSpawnTime += Time.deltaTime;
		if (currentSpawnTime >= spawnTime)
		{
			currentSpawnTime = 0f;
			Instantiate(ufo, PickLocation(5.5f), Quaternion.identity);
		}
	}

	public Vector2 PickLocation(float range)
	{
		Vector2 result = new Vector2();
		int maxAttempts = 1000;
		float x, y;
		bool isSafe;

		while (true)
		{
			for (int attempts = 0; attempts < maxAttempts; attempts++)
			{
				x = Random.Range(-Ship.halfScreen.x, Ship.halfScreen.x);
				y = Random.Range(-Ship.halfScreen.y, Ship.halfScreen.y);
				result = new Vector2(x, y);

				isSafe = true;

				if (Vector2.Distance(asteroid.transform.position, result) <= range)
				{
					isSafe = false;
					break;
				}

				if (isSafe)
					return result;
			}
		}
	}

	void OnDrawGizmosSelected()
	{
		Gizmos.color = Color.blue;

		Gizmos.DrawWireSphere(transform.position, asteroidSafeRegion);

	}

	internal GameObject SpawnAsteroid(GameObject asteroid, Vector2 position, Quaternion rotation)
	{
		GameObject newAsteroid = Instantiate(asteroid, position, rotation);

		int randomAsteroidIndex = Random.Range(0, asteroidSprites.Count);

		newAsteroid.GetComponent<SpriteRenderer>().sprite = asteroidSprites[randomAsteroidIndex];
	
		newAsteroid.GetComponent<Asteroid>().InitializeAsteroidComponents();

		return newAsteroid;
	}
}
