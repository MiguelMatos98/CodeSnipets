using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class RocketScript : MonoBehaviour
{
	public enum PlayerID
	{
		One,
		Two
	}
	public PlayerID playerID;

	public bool HasFirstCollided { get; private set; }

	public GameObject bangVFX;

	private Tilemap tilemap;

	private void OnTriggerEnter2D(Collider2D collision)
	{
		if (collision.CompareTag("Tank"))
		{
			if (this.playerID != collision.GetComponent<TankMovement>().playerID)
			{
				--collision.GetComponent<TankMovement>().health;
				if (collision.GetComponent<TankMovement>().health <= 0)
				{
					collision.gameObject.SetActive(false);
				}

				Instantiate(bangVFX, collision.gameObject.transform.position, Quaternion.identity);
			}
		}
		else if (collision.CompareTag("DestructibleTile"))
		{
			Instantiate(bangVFX, transform.position, Quaternion.identity);

			gameObject.SetActive(false);

			tilemap = collision.gameObject.GetComponent<Tilemap>();

			Vector3Int tilePosition = tilemap.WorldToCell(transform.position + transform.right * transform.localScale.x);

			tilemap.SetTile(tilePosition, null);
		}
		else if (collision.CompareTag("UndestructibleTile"))
		{
			RocketScript rocketScript = collision.GetComponent<RocketScript>();

			if (rocketScript == null || !rocketScript.HasFirstCollided)
				Instantiate(bangVFX, transform.position, Quaternion.identity);

			HasFirstCollided = true;

			gameObject.SetActive(false);
		}
	}
}
