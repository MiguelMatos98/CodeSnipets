using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody2D))]
[RequireComponent(typeof(CircleCollider2D))]
[RequireComponent(typeof(SpriteRenderer))]
public class Ball : MonoBehaviour
{
	private float speed = 6f;
	private float maxSpeed = 15f;
	private float speedIncreasePerHit = 1f;
	private float minSpeedRatio = 0.85f;

	private float verticalStrength = 1.8f;
	private float verticalHitRadius = 0.92f;
	private float verticalDirectionThreshold = 0.80f;
	private float verticalBounceDetectionThreshold = 0.18f;

	private float flatBounceNudgePerHit = 0.03f;
	private float maxFlatBounceNudge = 0.25f;

	private float sweepSafetyMultiplier = 1.15f;
	private int maxSweepIterations = 4;
	private float sweepSeparation = 0.025f;

	[SerializeField] private LayerMask sweepLayerMask;

	private float verticalWallLimit = 4.98f;

	private int maxPadHorizontalBounces = 2;
	private float padHorizontalDeflectionPerBounce = 0.12f;
	private float padMaxHorizontalDeflection = 0.65f;
	private float flatHitThreshold = 0.12f;

	private float naturalSpinStrength = 720f;
	private float paddleSpinStrength = 900f;

	private float collisionCooldown = 0.05f;
	private float bounceNormalThreshold = 0.95f;

	private float ghostSpacingDistance = 0.18f;
	private float finalGhostTipScale = 0.25f;
	private float ghostAlpha = 0.35f;

	private float goalExitDistance = 3.5f;

	private int consecutiveFlatWallBounces;
	private int padHorizontalBounceCount;
	private int padHorizontalDeflectionDir = 1;
	private int currentSpinDirection = -1;

	private float currentSpeed;
	private float padHorizontalDeflection;
	private float currentSpinStrength;
	private float lastBounceTime;

	private bool spinLockedByMovingPad;
	private bool ballMovementActive;
	private bool goalExitSequenceActive;

	private Vector2 lastBounceNormal;
	private Vector2 preCollisionVelocity;
	private Vector2 spawnPosition;
	private Vector2 lastValidDirection = Vector2.right;

	private struct TrailSample
	{
		public Vector2 position;
		public Quaternion rotation;
	}

	private List<TrailSample> positionHistory;
	private SpriteRenderer[] ghostRenderers;

	private SpriteRenderer ballSprite;
	private Rigidbody2D ballRigidbody;
	private CircleCollider2D circleCollider;

	private void Awake()
	{
		ballRigidbody = GetComponent<Rigidbody2D>();
		circleCollider = GetComponent<CircleCollider2D>();
		ballSprite = GetComponent<SpriteRenderer>();

		ballRigidbody.bodyType = RigidbodyType2D.Dynamic;
		ballRigidbody.gravityScale = 0f;
		ballRigidbody.collisionDetectionMode = CollisionDetectionMode2D.Continuous;
		ballRigidbody.interpolation = RigidbodyInterpolation2D.Interpolate;
		ballRigidbody.freezeRotation = false;

		spawnPosition = ballRigidbody.position;
		currentSpeed = speed;

		InitializeGhostTrail();
	}

	private void FixedUpdate()
	{
		if (ballMovementActive)
		{
			HardClampVelocity();

			preCollisionVelocity = ballRigidbody.linearVelocity;

			PerformPredictiveSweep();
			EmergencyWallContainment();
			ApplyContinuousSpin();
			EnforceMinimumSpeed();

			HardClampVelocity();

			if (positionHistory.Count == 0 || Vector2.Distance(ballRigidbody.position, positionHistory[0].position) >= ghostSpacingDistance)
			{
				RecordTrailSample();
			}
		}
		else if (goalExitSequenceActive)
		{
			ApplyContinuousSpin();
		}
		else
		{
			ballRigidbody.linearVelocity = Vector2.zero;
			ballRigidbody.angularVelocity = 0f;

			HideAllGhosts();
			
			return;
		}

		UpdateGhostTrail();
	}

	private void OnCollisionEnter2D(Collision2D collision)
	{
		if (!ballMovementActive || collision.contactCount == 0)
			return;

		ContactPoint2D contact = collision.GetContact(0);

		if (Time.time - lastBounceTime < collisionCooldown && Vector2.Dot(lastBounceNormal, contact.normal) > bounceNormalThreshold)
			return;

		if (preCollisionVelocity.sqrMagnitude > 0f && Vector2.Dot(preCollisionVelocity.normalized, contact.normal) >= 0.0f)
			return;

		ResolveBounce(contact.point, contact.normal, collision.collider);
	}

	private void HardClampVelocity()
	{
		float speed = ballRigidbody.linearVelocity.magnitude;

		if (speed > maxSpeed)
			ballRigidbody.linearVelocity = ballRigidbody.linearVelocity.normalized * maxSpeed;
	}

	private void PerformPredictiveSweep()
	{
		float radius = circleCollider.radius * transform.lossyScale.x;
		float remainingTime = Time.fixedDeltaTime;

		ResolveStartingOverlap();

		for (int iteration = 0; iteration < maxSweepIterations; iteration++)
		{
			Vector2 velocity = ballRigidbody.linearVelocity;
			float speed = velocity.magnitude;

			if (speed < 0.0001f)
				break;

			Vector2 direction = velocity.normalized;
			float travelDistance = speed * remainingTime;
			float castDistance = travelDistance + radius * sweepSafetyMultiplier;

			RaycastHit2D hit = Physics2D.CircleCast(ballRigidbody.position, radius, direction, castDistance, sweepLayerMask);

			if (hit.collider == null)
				break;

			if (hit.distance > travelDistance)
				break;

			if (Time.time - lastBounceTime < collisionCooldown && Vector2.Dot(lastBounceNormal, hit.normal) > bounceNormalThreshold)
				break;

			float timeToHit = hit.distance / speed;

			ballRigidbody.position = hit.centroid - hit.normal * sweepSeparation;
			
			Physics2D.SyncTransforms();

			preCollisionVelocity = ballRigidbody.linearVelocity;
			ballRigidbody.linearVelocity = Vector2.zero;

			ResolveBounce(hit.point, hit.normal, hit.collider);

			remainingTime -= timeToHit;

			if (remainingTime <= 0.000001f)
				break;

			ballRigidbody.position += hit.normal * sweepSeparation;

			Physics2D.SyncTransforms();
		}
	}

	private void ResolveStartingOverlap()
	{
		float radius = circleCollider.radius * transform.lossyScale.x;

		Collider2D[] overlaps = Physics2D.OverlapCircleAll(ballRigidbody.position, radius, sweepLayerMask);

		if (overlaps.Length == 0)
			return;

		foreach (Collider2D other in overlaps)
		{
			if (other == circleCollider)
				continue;

			ColliderDistance2D distance = circleCollider.Distance(other);

			if (!distance.isOverlapped)
				continue;

			Vector2 normal = distance.normal;

			if (normal.sqrMagnitude < 0.000001f)
				continue;

			normal.Normalize();

			float penetration = Mathf.Abs(distance.distance);

			ballRigidbody.position += normal * (penetration + sweepSeparation);

			Physics2D.SyncTransforms();

			if (ballRigidbody.linearVelocity.sqrMagnitude > 0.0001f && Vector2.Dot(ballRigidbody.linearVelocity.normalized, normal) < 0f)
			{ 
				preCollisionVelocity = ballRigidbody.linearVelocity;
				ballRigidbody.linearVelocity = Vector2.zero;

				ResolveBounce(ballRigidbody.position, normal, other);
			}
		}
	}

	private void EmergencyWallContainment()
	{
		float radius = circleCollider.radius * transform.lossyScale.x;

		Vector2 position = ballRigidbody.position;
		Vector2 velocity = ballRigidbody.linearVelocity;

		bool corrected = false;

		if (position.y + radius > verticalWallLimit)
		{
			position.y = verticalWallLimit - radius;

			if (velocity.y > 0.0f)
			{
				velocity.y = -Mathf.Abs(velocity.y);
				corrected = true;
			}
		}

		if (position.y - radius < -verticalWallLimit)
		{
			position.y = -verticalWallLimit + radius;

			if (velocity.y < 0.0f)
			{
				velocity.y = Mathf.Abs(velocity.y);
				corrected = true;
			}
		}

		if (!corrected)
			return;

		ballRigidbody.position = position;

		if (velocity.sqrMagnitude > 0.0001f)
		{
			velocity = velocity.normalized * Mathf.Clamp(velocity.magnitude, speed, maxSpeed);

			ballRigidbody.linearVelocity = velocity;
			lastValidDirection = velocity.normalized;
		}
	}

	private void EnforceMinimumSpeed()
	{
		float speed = ballRigidbody.linearVelocity.magnitude;
		float minimumSpeed = currentSpeed * minSpeedRatio;

		if (speed >= minimumSpeed)
		{
			if (speed > 0.01f)
				lastValidDirection = ballRigidbody.linearVelocity.normalized;

			return;
		}

		Vector2 direction = speed > 0.01f ? ballRigidbody.linearVelocity.normalized : lastValidDirection;

		ballRigidbody.linearVelocity = direction * currentSpeed;
	}

	private void ResolveBounce(Vector2 point, Vector2 normal, Collider2D collider)
	{
		lastBounceTime = Time.time;
		lastBounceNormal = normal;

		if (collider.CompareTag("Pad"))
		{
			if (!collider.TryGetComponent(out Pad pad))
				return;

			Vector2 exitDirection = CalculatePadBounce(point, collider);
			ballRigidbody.linearVelocity = exitDirection * speed;

			lastValidDirection = exitDirection;

			ApplyPadSpin(pad);

			float clampedSpeed = Mathf.Clamp(currentSpeed, speed, maxSpeed);

			currentSpeed = Mathf.Clamp(currentSpeed + speedIncreasePerHit, speed, maxSpeed);
		}
		else
		{
			HandleWallCollision(normal);
		}

		SFXManager.Instance.PlaySoundEffect(SFXType.BallBounce);
	}

	private Vector2 CalculatePadBounce(Vector2 contactPoint, Collider2D padCollider)
	{
		Bounds bounds = padCollider.bounds;

		float hitPosition = Mathf.Clamp((contactPoint.y - bounds.center.y) / bounds.extents.y, -1f, 1f);

		float faceX = bounds.center.x < 0.0f ? 1.0f : -1.0f;

		Vector2 direction = new Vector2(faceX, hitPosition * verticalStrength).normalized;

		if (Mathf.Abs(hitPosition) >= verticalHitRadius && Mathf.Abs(direction.x) <= verticalDirectionThreshold)
			direction = new Vector2(0f, Mathf.Sign(hitPosition));

		bool isFlatHit = Mathf.Abs(direction.y) < flatHitThreshold;

		if (isFlatHit)
		{
			padHorizontalBounceCount++;

			if (padHorizontalBounceCount > maxPadHorizontalBounces)
			{
				padHorizontalDeflection += padHorizontalDeflectionPerBounce;

				padHorizontalDeflection = Mathf.Min(padHorizontalDeflection, padMaxHorizontalDeflection);

				padHorizontalDeflectionDir *= -1;

				direction = new Vector2(direction.x, padHorizontalDeflectionDir * padHorizontalDeflection).normalized;
			}
		}
		else
		{
			padHorizontalBounceCount = 0;
			padHorizontalDeflection = 0.0f;
		}

		return direction;
	}

	private void ApplyPadSpin(Pad pad)
	{
		float padVelocityY = pad.CurrentVelocity.y;
		bool isRightPad = pad.transform.position.x > 0.0f;

		if (padVelocityY == 0.0f)
		{
			currentSpinDirection = isRightPad ? 1 : -1;

			currentSpinStrength = naturalSpinStrength;
			
			spinLockedByMovingPad = false;
		}
		else if (padVelocityY > 0.0f)
		{
			currentSpinDirection = isRightPad ? 1 : -1;

			currentSpinStrength = paddleSpinStrength;
			
			spinLockedByMovingPad = true;
		}
		else
		{
			currentSpinDirection = isRightPad ? -1 : 1;

			currentSpinStrength = paddleSpinStrength;
			
			spinLockedByMovingPad = true;
		}
	}

	private void ApplyContinuousSpin()
	{
		if (!spinLockedByMovingPad)
		{
			if (ballRigidbody.linearVelocity.x > 0.05f)
				currentSpinDirection = -1;
			else if (ballRigidbody.linearVelocity.x < -0.05f)
				currentSpinDirection = 1;
		}

		if (currentSpinStrength <= 0.0f)
			currentSpinStrength = naturalSpinStrength;

		ballRigidbody.angularVelocity = currentSpinDirection * currentSpinStrength;
	}

	private void ApplySpawnSpin()
	{
		currentSpinDirection = ballRigidbody.linearVelocity.x > 0.0f ? -1 : 1;

		currentSpinStrength = naturalSpinStrength;
		
		spinLockedByMovingPad = false;
		
		ApplyContinuousSpin();
	}

	private void HandleWallCollision(Vector2 normal)
	{
		Vector2 incomingDirection = preCollisionVelocity.sqrMagnitude > 0.0001f ? preCollisionVelocity.normalized : lastValidDirection;

		Vector2 reflection = Vector2.Reflect(incomingDirection, normal).normalized;

		bool isTopBottomWall = Mathf.Abs(normal.y) > 0.9f;

		if (isTopBottomWall)
		{
			float horizontalAmount = Mathf.Abs(reflection.x);

			if (horizontalAmount < verticalBounceDetectionThreshold)
			{
				consecutiveFlatWallBounces++;

				float nudge = Mathf.Min(flatBounceNudgePerHit * consecutiveFlatWallBounces, maxFlatBounceNudge);

				float sign = reflection.x != 0f ? Mathf.Sign(reflection.x) : (Random.value < 0.5f ? -1f : 1f);

				reflection = new Vector2(reflection.x + sign * nudge, reflection.y).normalized;
			}
			else
				consecutiveFlatWallBounces = 0;
		}
		else
			consecutiveFlatWallBounces = 0;

		ballRigidbody.linearVelocity = reflection * speed;
		lastValidDirection = reflection;
	}

	private void InitializeGhostTrail()
	{
		positionHistory = new List<TrailSample>();

		int ghostCount = (int)maxSpeed + 1; 

		ghostRenderers = new SpriteRenderer[ghostCount];

		GameObject container = new GameObject("GhostTrail");

		container.transform.SetParent(transform.parent);

		for (int i = 0; i < ghostRenderers.Length; i++)
		{
			GameObject ghost = new GameObject("Ghost_" + i);

			ghost.transform.SetParent(container.transform);

			SpriteRenderer sprite = ghost.AddComponent<SpriteRenderer>();

			sprite.enabled = false;
			sprite.sprite = ballSprite.sprite;
			sprite.sortingLayerID = ballSprite.sortingLayerID;
			sprite.sortingOrder = ballSprite.sortingOrder - 1;

			ghostRenderers[i] = sprite;
		}
	}

	private void RecordTrailSample()
	{	
		positionHistory.Insert(0, new TrailSample { position = ballRigidbody.position, rotation = transform.rotation });

		if (positionHistory.Count > ghostRenderers.Length)
			positionHistory.RemoveAt(positionHistory.Count - 1);
	}

	private void UpdateGhostTrail()
	{
		int fullSlots = Mathf.Clamp(Mathf.FloorToInt(currentSpeed), 0, ghostRenderers.Length);

		float fraction = currentSpeed - fullSlots;
		
		int totalVisibleSlots = fraction > 0.0f ? Mathf.Min(fullSlots + 1, ghostRenderers.Length) : fullSlots;

		for (int i = 0; i < ghostRenderers.Length; i++)
		{
			SpriteRenderer ghost = ghostRenderers[i];

			if (i >= totalVisibleSlots || i >= positionHistory.Count)
			{
				ghost.enabled = false;
				continue;
			}

			TrailSample sample = positionHistory[i];

			ghost.transform.position = sample.position;
			ghost.transform.rotation = sample.rotation;

			float normalizedIndex = totalVisibleSlots <= 1.0f ? 0.0f : (float)i / (totalVisibleSlots - 1);

			float scale = Mathf.Lerp(1.0f, finalGhostTipScale, normalizedIndex);

			ghost.transform.localScale = transform.root.localScale * scale;

			bool isLastVisible = i == totalVisibleSlots - 1;
			
			float slotAlpha = isLastVisible && fraction > 0.0f ? fraction : 1.0f;

			Color spriteColor = ballSprite.color;
			spriteColor.a = ghostAlpha * slotAlpha;
			ghost.color = spriteColor;

			ghost.enabled = true;
		}
	}

	private void HideAllGhosts()
	{
		if (ghostRenderers == null) 
			return;

		foreach (SpriteRenderer ghost in ghostRenderers)
			ghost.enabled = false;
	}

	public void ClearGhosts()
	{
		positionHistory.Clear();
		HideAllGhosts();
	}

	public void ResetBallSpeed()
	{
		currentSpeed = speed;
	}

	public void StopBall()
	{
		ballMovementActive = false;

		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0f;
	}

	public Rigidbody2D GetBallRigidbody()
	{
		return ballRigidbody;
	}

	public IEnumerator StopBallAndWaitForGhosts()
	{
		if (goalExitSequenceActive)
			yield break;

		goalExitSequenceActive = true;
		ballMovementActive = false;

		Vector2 exitVelocity = ballRigidbody.linearVelocity;

		float exitSpeed = exitVelocity.magnitude;

		if (exitSpeed < 0.01f)
			exitSpeed = currentSpeed;

		Vector2 exitDirection = exitVelocity.sqrMagnitude > 0.01f ? exitVelocity.normalized : lastValidDirection.normalized;

		circleCollider.enabled = false;

		ballRigidbody.linearVelocity = Vector2.zero;

		ApplyContinuousSpin();

		float exitDistanceTraveled = 0f;

		while (exitDistanceTraveled < goalExitDistance)
		{
			float step = exitSpeed * Time.fixedDeltaTime;

			ballRigidbody.position += exitDirection * step;
			exitDistanceTraveled += step;

			if (positionHistory.Count == 0 || Vector2.Distance(ballRigidbody.position, positionHistory[0].position) >= ghostSpacingDistance)
			{
				RecordTrailSample();
			}

			yield return new WaitForFixedUpdate();
		}

		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0f;

		ClearGhosts();

		goalExitSequenceActive = false;
	}

	public IEnumerator MovementDelayCoroutine(Vector2 direction, float delay)
	{
		yield return new WaitForSeconds(delay);

		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0f;

		ballRigidbody.position = spawnPosition;

		Physics2D.SyncTransforms();

		ballMovementActive = true;

		circleCollider.enabled = true;

		ResolveStartingOverlap();

		Vector2 spawnDirection = direction.sqrMagnitude > 0.0001f ? direction.normalized : lastValidDirection;

		ballRigidbody.linearVelocity = spawnDirection * Mathf.Clamp(currentSpeed, speed, maxSpeed);

		ClearGhosts();

		RecordTrailSample();

		ApplySpawnSpin();

		SFXManager.Instance.PlaySoundEffect(SFXType.BallRespawn);
	}

	public Vector2 GetSpawnPosition()
	{
		return spawnPosition;
	}
}
