using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Transactions;
using UnityEngine;

public class FruitManager : MonoBehaviour
{
    public List<GameObject> spawnableItems;

	private List<GameObject> sceneItems = new List<GameObject>();

    public float fixedSpawnInterval;

    private float currentSpawnInterval = 0.0f;

    [SerializeField] private float itemSafeRegion = 0.5f;

	// Update is called once per frame
	void Update()
    {
		currentSpawnInterval += Time.deltaTime;

        if (currentSpawnInterval > fixedSpawnInterval)
		{
			currentSpawnInterval = 0;

			int rand = Random.Range(0, spawnableItems.Count);
			
			Vector2 spawnPosition = PickLocation(itemSafeRegion);

			GameObject item = Instantiate(spawnableItems[rand], spawnPosition, Quaternion.identity);
		
			sceneItems.Add(item);
		}
	}

	private Vector2 PickLocation(float range)
	{
		Vector2 result = new Vector2();
		int maxAttempts = 1000;
		float x, y;
		bool isSafe;

		while (true)
		{
			for (int attempts = 0; attempts < maxAttempts; attempts++)
			{
				x = Random.Range(-SnakeHead.halfScreen.x, SnakeHead.halfScreen.x);
				y = Random.Range(-SnakeHead.halfScreen.y, SnakeHead.halfScreen.y);
				result = new Vector2(x, y);

				isSafe = true;

				foreach (GameObject item in sceneItems)
				{

					if (Vector2.Distance(item.transform.position, result) <= range)
					{
						isSafe = false;
						break;
					}
				}

				if (isSafe)
					return result;
			}
		}
	}

	void OnDrawGizmosSelected()
	{
		Gizmos.color = Color.red;

		Gizmos.DrawWireSphere(transform.position, itemSafeRegion);

	}
}
