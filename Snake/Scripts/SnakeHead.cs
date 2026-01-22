using System.Collections;
using System.Collections.Generic;
using System.Net;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.VFX;
using static TMPro.SpriteAssetUtilities.TexturePacker_JsonArray;

public class SnakeHead : MonoBehaviour
{
	private int appleScore = 0;
	private int orangeScore = 0;
	private int coinScore = 0;

	public float moveSpeed;
    public float turnSpeed;

    public float frameDelay;

	public static Vector2 halfScreen;

	public Text aplleScoreText;
	public Text orangeScoreText;
	public Text coinScoreText;

	private Rigidbody2D rigidBody;

	private SpriteRenderer spriteRenderer;

	private List<SpriteRenderer> bodySpriteRenderers;

	public List<GameObject> bodyParts;

    public GameObject newBodyPart;

	public Camera mainCamera;

	public float animationTimer = 2.0f;

	public ParticleSystem starParticles;

	private void Awake()
	{
		rigidBody = GetComponent<Rigidbody2D>();

		spriteRenderer = GetComponent<SpriteRenderer>();

		bodySpriteRenderers = new List<SpriteRenderer>(bodyParts.Count);

		foreach (GameObject part in bodyParts)
			bodySpriteRenderers.Add(part.GetComponent<SpriteRenderer>());

		halfScreen = new Vector2(mainCamera.aspect * mainCamera.orthographicSize, mainCamera.orthographicSize);
	}

    void Update()
    {
		rigidBody.linearVelocity = transform.up * moveSpeed;

        if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow))
            transform.Rotate(0, 0, turnSpeed * Time.deltaTime);

        if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow))
            transform.Rotate(0, 0, -turnSpeed * Time.deltaTime);

        StartCoroutine(PropagateToNextBodyPart(transform.position));

		for (int bodyPartIndex = 0; bodyPartIndex < bodyParts.Count; bodyPartIndex++)
        {
            Vector3 bodyPartDestinaton = bodyPartIndex == 0 ? transform.position : bodyParts[bodyPartIndex - 1].transform.position;

            bodyParts[bodyPartIndex].transform.up = bodyPartDestinaton - bodyParts[bodyPartIndex].transform.position;
		}

		Vector2 relitiveCameraDist = transform.position - mainCamera.transform.position; 

		if (relitiveCameraDist.x < -halfScreen.x)
		{
			transform.position += Vector3.right * (halfScreen.x * 2);
		}
		else if (relitiveCameraDist.x > halfScreen.x)
		{
			transform.position += Vector3.left * (halfScreen.x * 2);
		}

		if (relitiveCameraDist.y < -halfScreen.y)
		{
			transform.position += Vector3.up * (halfScreen.y * 2);
		}
		else if (relitiveCameraDist.y > halfScreen.y)
		{
			transform.position += Vector3.down * (halfScreen.y * 2);
		}

	}

	public IEnumerator PropagateToNextBodyPart(Vector3 previousPosition, int bodyPartIndex=-1)
	{
		bodyPartIndex++;

		yield return new WaitForSeconds(frameDelay);

		PropagationLogic(bodyPartIndex, previousPosition);
	}

	private void PropagationLogic(int bodyPartIndex, Vector3 previousPosition)
	{
		bodyParts[bodyPartIndex].transform.position = previousPosition;

		if (bodyParts.Count > bodyPartIndex + 1)
		{
			StartCoroutine(PropagateToNextBodyPart(bodyParts[bodyPartIndex].transform.position, bodyPartIndex));
		}
	}

	private void OnTriggerEnter2D(Collider2D collision)
	{
		if (collision.gameObject.CompareTag("Death"))
		{
			rigidBody.linearVelocity = Vector2.zero;
			
			StopAllCoroutines();

			StartCoroutine(Dying());
			StopCoroutine(Dying());

			SFXManager.Instance.audioSource.Stop();

			SFXManager.Instance.PlaySFX(SFXManager.Sounds.Lose);

			enabled = false;
		}

		if (collision.gameObject.CompareTag("Items"))
		{
			int bodyPartIndex = bodyParts.Count - 2;

			CollectableItem collectedItem = collision.gameObject.GetComponent<CollectableItem>();

			CollectableItem.ItemType itemType = collectedItem.itemType;

			switch (itemType)
			{
				case (CollectableItem.ItemType.Apple):
					appleScore++;
					aplleScoreText.text = appleScore.ToString();
					break;
				case (CollectableItem.ItemType.Orange):
					orangeScore++;
					orangeScoreText.text = orangeScore.ToString();
					break;
				case (CollectableItem.ItemType.Coin):
					coinScore++;
					coinScoreText.text = coinScore.ToString();
					break;
			}

			Destroy(Instantiate(starParticles, collision.transform.position, Quaternion.identity), starParticles.main.startLifetimeMultiplier);

			SFXManager.Instance.PlaySFX(SFXManager.Sounds.Eat);

			collision.gameObject.SetActive(false);

			GameObject newBodyPartInstance = Instantiate(newBodyPart, bodyParts[bodyParts.Count - 1].transform.position, bodyParts[bodyParts.Count - 1].transform.rotation);

			bodyParts.Insert(bodyPartIndex, newBodyPartInstance);

			bodySpriteRenderers.Insert(bodyPartIndex, newBodyPartInstance.GetComponent<SpriteRenderer>());
		}
	}

	public IEnumerator Dying()
	{
		float shrinkSpeed = 1.0f;
		float fadeSpeed = 1.0f;

		// Looping until the snake is both tiny and fully transparent
		while (transform.localScale.x > 0.0f)
		{
			// shrink snake head 
			Vector3 headScale = transform.localScale;

			headScale.x -= shrinkSpeed * Time.deltaTime;
			headScale.y -= shrinkSpeed * Time.deltaTime;
			headScale.z -= shrinkSpeed * Time.deltaTime;

			transform.localScale = headScale;

			// shrink every snake body part
			for (int bodyPartIndex = 0; bodyPartIndex < bodyParts.Count; bodyPartIndex++)
			{
				Transform bodyPartTransform = bodyParts[bodyPartIndex].transform;

				Vector3 bodyPartScale = bodyPartTransform.localScale;

				bodyPartScale.x -= shrinkSpeed * Time.deltaTime;
				bodyPartScale.y -= shrinkSpeed * Time.deltaTime;
				bodyPartScale.z -= shrinkSpeed * Time.deltaTime;

				bodyPartTransform.localScale = bodyPartScale;
			}

			// fade out snake head
			Color headSpriteColor = spriteRenderer.color;

			headSpriteColor.a -= fadeSpeed * Time.deltaTime;

			spriteRenderer.color = headSpriteColor;

			// fade out every snake body part
			for (int bodyPartIndex = 0; bodyPartIndex < bodyParts.Count; bodyPartIndex++)
			{
				Color bodySpriteColor = bodySpriteRenderers[bodyPartIndex].color;

				bodySpriteColor.a -= fadeSpeed * Time.deltaTime;

				bodySpriteRenderers[bodyPartIndex].color = bodySpriteColor;
			}

			yield return null;
		}

		// Destroy the snake head and all body parts
		Destroy(gameObject);
		for (int bodyPartIndex = 0; bodyPartIndex < bodyParts.Count; bodyPartIndex++)
			Destroy(bodyParts[bodyPartIndex]);

	}
}
