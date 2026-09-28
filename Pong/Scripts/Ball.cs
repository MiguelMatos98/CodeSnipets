using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody2D))]
public class Ball : MonoBehaviour
{
	private float speed = 6f;
	private float maxSpeed = 15f;
	private float speedIncreasePerHit = 1f;
	private float minSpeedRatio = 0.85f;

	private float ballScale = 0.5f;
	private float colliderRadius = 0.31f;

	private float verticalStrength = 1.8f;
	private float verticalHitZone = 0.92f;
	private float verticalDirectionThreshold = 0.80f;
	private float verticalBounceDetectionThreshold = 0.18f;

	private float flatBounceNudgePerHit = 0.03f;
	private float maxFlatBounceNudge = 0.25f;
	private float wallSeparation = 0.02f;

	private float sweepSafetyMultiplier = 1.15f;
	private int maxSweepIterations = 4;
	private float sweepSeparation = 0.025f;

	private float verticalWallLimit = 4.98f;
	private float emergencyWallMargin = 0.02f;

	private int maxPadHorizontalBounces = 2;
	private float padHorizontalDeflectionPerBounce = 0.12f;
	private float padMaxHorizontalDeflection = 0.65f;
	private float flatHitThreshold = 0.12f;

	private float naturalSpinStrength = 720f;
	private float paddleSpinStrength = 900f;

	private float wallSpeedRetention = 1f;

	private float collisionCooldown = 0.05f;
	private float bounceNormalThreshold = 0.95f;

	private float ghostSpacingDistance = 0.18f;
	private float finalGhostTipScale = 0.25f;
	private float ghostAlpha = 0.35f;

	private float goalExitDistance = 3.5f;
	private float leftScoreBarrierX = -8.94f;
	private float rightScoreBarrierX = 8.952f;

	private int consecutiveFlatWallBounces;
	private int padHorizontalBounceCount;
	private int maxGhostSlots;

	private int padHorizontalDeflectionDir = 1;
	private int currentSpinDirection = -1;

	private float currentSpeed;
	private float padHorizontalDeflection;
	private float currentSpinStrength;
	private float lastBounceTime;
	private float defaultBallSpeed;

	private bool spinLockedByMovingPad;
	private bool ballMovementActive;
	private bool goalExitSequenceActive;

	private Vector2 lastBounceNormal;
	private Vector2 preCollisionVelocity;
	private Vector2 spawnPosition;
	private Vector2 lastValidDirection = Vector2.right;

	private LayerMask sweepLayerMask;

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

		transform.root.localScale = Vector3.one * ballScale;

		if (circleCollider != null)
		{
			circleCollider.radius = colliderRadius;
		}

		ballRigidbody.bodyType = RigidbodyType2D.Dynamic;
		ballRigidbody.gravityScale = 0f;
		ballRigidbody.collisionDetectionMode = CollisionDetectionMode2D.Continuous;
		ballRigidbody.interpolation = RigidbodyInterpolation2D.Interpolate;
		ballRigidbody.freezeRotation = false;

		spawnPosition = ballRigidbody.position;
		defaultBallSpeed = speed;
		currentSpeed = speed;
		lastBounceTime = -999f;

		maxGhostSlots = Mathf.CeilToInt(maxSpeed) + 1;

		InitializeGhostTrail();
	}

	private void FixedUpdate()
	{
		if (ballRigidbody == null) 
			return;

		if (ballMovementActive)
			HardClampVelocity();

		preCollisionVelocity = ballRigidbody.linearVelocity;

		if (ballMovementActive)
		{
			PerformPredictiveSweep();
			EmergencyWallContainment();
		}

		if (ballMovementActive || goalExitSequenceActive)
			ApplyContinuousSpin();
		else
			ballRigidbody.angularVelocity = 0f;

		if (!ballMovementActive)
		{
			if (!goalExitSequenceActive)
			{
				ballRigidbody.linearVelocity = Vector2.zero;
				ballRigidbody.angularVelocity = 0f;
			}
			UpdateGhostTrail();
			return;
		}

		EnforceMinimumSpeed();
		HardClampVelocity();

		if (positionHistory.Count == 0 || Vector2.Distance(ballRigidbody.position, positionHistory[0].position) >= ghostSpacingDistance)
			RecordTrailSample();

		UpdateGhostTrail();
	}

	private void HardClampVelocity()
	{
		if (ballRigidbody == null) 
			return;

		float speed = ballRigidbody.linearVelocity.magnitude;
		if (speed > maxSpeed)
			ballRigidbody.linearVelocity = ballRigidbody.linearVelocity.normalized * maxSpeed;
	}

	private void PerformPredictiveSweep()
	{
		if (circleCollider == null)
			return;

		float radius = circleCollider.radius * transform.lossyScale.x;

		float remainingTime = Time.fixedDeltaTime;

		ResolveStartingOverlap();

		for (int iteration = 0; iteration < maxSweepIterations; iteration++)
		{
			Vector2 velocity = ballRigidbody.linearVelocity;

			float speed = velocity.magnitude;

			if (speed < 0.0001f)
				break;

			if (remainingTime <= 0.000001f)
				break;

			Vector2 direction = velocity / speed;

			float travelDistance = speed * remainingTime;

			float safetyDistance = radius * sweepSafetyMultiplier;

			float castDistance = travelDistance + safetyDistance;

			RaycastHit2D hit = Physics2D.CircleCast(ballRigidbody.position, radius, direction, castDistance, sweepLayerMask);

			if (hit.collider == null)
				break;

			if (Time.time - lastBounceTime < collisionCooldown && Vector2.Dot(lastBounceNormal, hit.normal) > bounceNormalThreshold)
				break;

			if (hit.distance > travelDistance)
				break;

			float distanceToHit = Mathf.Max(0f, hit.distance);

			float timeToHit = distanceToHit / speed;

			ballRigidbody.position = hit.centroid - hit.normal * sweepSeparation;

			Physics2D.SyncTransforms();

			preCollisionVelocity = ballRigidbody.linearVelocity;

			ballRigidbody.linearVelocity = Vector2.zero;

			ResolveBounce(hit.point, hit.normal, hit.collider);

			HardClampVelocity();

			remainingTime -= timeToHit;

			if (remainingTime <= 0.000001f)
				break;

			ballRigidbody.position += hit.normal * sweepSeparation;

			Physics2D.SyncTransforms();
		}

		HardClampVelocity();
	}

	private void ResolveStartingOverlap()
	{
		if (circleCollider == null)
			return;

		float radius = circleCollider.radius * transform.lossyScale.x;

		Collider2D[] overlaps = Physics2D.OverlapCircleAll(ballRigidbody.position, radius, sweepLayerMask);

		if (overlaps == null || overlaps.Length == 0)
			return;

		foreach (Collider2D other in overlaps)
		{
			if (other == null || other == circleCollider)
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

				HardClampVelocity();
			}
		}
	}

	private void EmergencyWallContainment()
	{
		if (ballRigidbody == null || !ballMovementActive) 
			return;

		float radius = circleCollider != null ? circleCollider.radius * transform.lossyScale.x : colliderRadius;

		Vector2 position = ballRigidbody.position;
		Vector2 velocity = ballRigidbody.linearVelocity;
		bool corrected = false;

		if (position.y + radius > verticalWallLimit)
		{
			position.y = verticalWallLimit - radius - emergencyWallMargin;
			if (velocity.y > 0f)
			{
				velocity.y = -Mathf.Abs(velocity.y);
				corrected = true;
			}
		}

		if (position.y - radius < -verticalWallLimit)
		{
			position.y = -verticalWallLimit + radius + emergencyWallMargin;
			if (velocity.y < 0f)
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

	private void OnCollisionEnter2D(Collision2D collision)
	{
		if (!ballMovementActive || collision.contactCount == 0) 
			return;

		ContactPoint2D contact = collision.GetContact(0);

		if (Time.time - lastBounceTime < collisionCooldown && Vector2.Dot(lastBounceNormal, contact.normal) > bounceNormalThreshold)
			return;

		if (preCollisionVelocity.sqrMagnitude > 0f && Vector2.Dot(preCollisionVelocity.normalized, contact.normal) >= 0f)
			return;

		ResolveBounce(contact.point, contact.normal, collision.collider);
	}

	private void ResolveBounce(Vector2 point, Vector2 normal, Collider2D collider)
	{
		lastBounceTime = Time.time;
		lastBounceNormal = normal;

		if (collider.CompareTag("Pad"))
		{
			Pad pad = collider.GetComponent<Pad>();
			if (pad == null) return;

			Vector2 exitDirection = CalculatePadBounce(point, collider);
			float clampedSpeed = Mathf.Clamp(currentSpeed, speed, maxSpeed);

			ballRigidbody.linearVelocity = exitDirection * speed;
			lastValidDirection = exitDirection;

			ApplyPadSpin(pad);
			IncreaseBallSpeed();
			PlayBounceSound();

			ballRigidbody.position += normal * wallSeparation;
			Physics2D.SyncTransforms();
			HardClampVelocity();
		}
		else
		{
			HandleWallCollision(normal);
			PlayBounceSound();

			ballRigidbody.position += normal * wallSeparation;
			Physics2D.SyncTransforms();
			HardClampVelocity();
		}
	}

	private void PlayBounceSound()
	{
		if (SFXManager.Instance != null) 
			SFXManager.Instance.PlaySoundEffect(SFXType.BallBounce);
	}

	private void PlaySpawnSound()
	{
		if (SFXManager.Instance != null) 
			SFXManager.Instance.PlaySoundEffect(SFXType.BallRespawn);
	}

	private Vector2 CalculatePadBounce(Vector2 contactPoint, Collider2D padCollider)
	{
		Bounds bounds = padCollider.bounds;
		float hitPosition = Mathf.Clamp((contactPoint.y - bounds.center.y) / bounds.extents.y, -1f, 1f);
		float faceX = bounds.center.x < 0f ? 1f : -1f;
		float verticalComponent = hitPosition * verticalStrength;

		Vector2 direction = new Vector2(faceX, verticalComponent).normalized;

		if (Mathf.Abs(hitPosition) >= verticalHitZone && Mathf.Abs(direction.x) <= verticalDirectionThreshold)
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
			padHorizontalDeflection = 0f;
		}

		return direction;
	}

	private void ApplyPadSpin(Pad pad)
	{
		if (pad == null) return;

		float padVelocityY = pad.CurrentVelocity.y;
		bool isRightPad = pad.transform.position.x > 0f;

		if (Mathf.Abs(padVelocityY) < 0.05f)
		{
			currentSpinDirection = isRightPad ? 1 : -1;
			currentSpinStrength = naturalSpinStrength;
			spinLockedByMovingPad = false;
			return;
		}

		if (padVelocityY > 0f)
		{
			currentSpinDirection = isRightPad ? 1 : -1;
			currentSpinStrength = paddleSpinStrength;
			spinLockedByMovingPad = true;
			return;
		}

		if (padVelocityY < 0f)
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

		if (currentSpinStrength <= 0f)
			currentSpinStrength = naturalSpinStrength;

		ballRigidbody.angularVelocity = currentSpinDirection * currentSpinStrength;
	}

	private void ApplySpawnSpin()
	{
		currentSpinDirection = ballRigidbody.linearVelocity.x > 0f ? -1 : 1;
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

		float clampedSpeed = Mathf.Clamp(currentSpeed * wallSpeedRetention, speed, maxSpeed);
		ballRigidbody.linearVelocity = reflection * speed;
		lastValidDirection = reflection;
	}

	private void IncreaseBallSpeed()
	{
		currentSpeed = Mathf.Clamp(currentSpeed + speedIncreasePerHit, speed, maxSpeed);
	}

	private void InitializeGhostTrail()
	{
		positionHistory = new List<TrailSample>();
		ghostRenderers = new SpriteRenderer[maxGhostSlots];

		GameObject container = new GameObject("GhostTrail");
		container.transform.SetParent(transform.parent);

		for (int i = 0; i < maxGhostSlots; i++)
		{
			GameObject ghost = new GameObject("Ghost_" + i);
			ghost.transform.SetParent(container.transform);

			SpriteRenderer sr = ghost.AddComponent<SpriteRenderer>();
			sr.enabled = false;

			if (ballSprite != null)
			{
				sr.sprite = ballSprite.sprite;
				sr.sortingLayerID = ballSprite.sortingLayerID;
				sr.sortingOrder = ballSprite.sortingOrder - 1;
			}

			ghostRenderers[i] = sr;
		}
	}

	private void RecordTrailSample()
	{
		if (ballRigidbody == null) 
			return;
		
		if (!ballMovementActive && !goalExitSequenceActive) 
			return;

		positionHistory.Insert(0, new TrailSample { position = ballRigidbody.position, rotation = transform.rotation });

		if (positionHistory.Count > maxGhostSlots)
			positionHistory.RemoveAt(positionHistory.Count - 1);
	}

	private void UpdateGhostTrail()
	{
		if (!ballMovementActive && !goalExitSequenceActive)
		{
			HideAllGhosts();
			return;
		}

		int fullSlots = Mathf.Clamp(Mathf.FloorToInt(currentSpeed), 0, maxGhostSlots);
		float fraction = currentSpeed - fullSlots;
		int totalVisibleSlots = fraction > 0f ? Mathf.Min(fullSlots + 1, maxGhostSlots) : fullSlots;

		for (int i = 0; i < maxGhostSlots; i++)
		{
			SpriteRenderer ghost = ghostRenderers[i];
			if (ghost == null) 
				continue;

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

			bool isLastVisible = (i == totalVisibleSlots - 1);
			float slotAlpha = (isLastVisible && fraction > 0f) ? fraction : 1f;

			if (ballSprite != null)
			{
				Color c = ballSprite.color;
				c.a = ghostAlpha * slotAlpha;
				ghost.color = c;
			}

			ghost.enabled = true;
		}
	}

	private void HideAllGhosts()
	{
		if (ghostRenderers == null) 
			return;
		
		foreach (var ghost in ghostRenderers)
		{
			if (ghost != null) 
				ghost.enabled = false;
		}
	}

	public void ClearGhosts()
	{
		positionHistory?.Clear();
		HideAllGhosts();
	}

	public IEnumerator StopBallAndWaitForGhosts()
	{
		if (goalExitSequenceActive) 
			yield break;

		goalExitSequenceActive = true;
		ballMovementActive = false;

		Vector2 exitVelocity = ballRigidbody.linearVelocity;
		float exitSpeed = exitVelocity.magnitude;
		if (exitSpeed < 0.01f) exitSpeed = currentSpeed;

		Vector2 exitDirection = exitVelocity.sqrMagnitude > 0.01f ? exitVelocity.normalized : lastValidDirection.normalized;

		if (circleCollider != null) 
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
				RecordTrailSample();

			yield return new WaitForFixedUpdate();
		}

		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0f;
		ClearGhosts();
		goalExitSequenceActive = false;
	}

	public bool HasPassedLeftScoreBarrier() 
	{ 
		return ballRigidbody != null && ballRigidbody.position.x <= leftScoreBarrierX; 
	}
	
	public bool HasPassedRightScoreBarrier()
	{ 
		return ballRigidbody != null && ballRigidbody.position.x >= rightScoreBarrierX; 
	}
	
	public float GetLeftScoreBarrierX() 
	{ 
		return leftScoreBarrierX; 
	}
	
	public float GetRightScoreBarrierX()
	{ 
		return rightScoreBarrierX; 
	}
	
	public Vector2 GetPosition() 
	{ 
		return ballRigidbody != null ? ballRigidbody.position : (Vector2)transform.position; 
	}

	public float GetRadius() 
	{ 
		return circleCollider != null ? circleCollider.radius * transform.lossyScale.x : 0.25f; 
	}

	public void ResetBallSpeed()
	{
		currentSpeed = defaultBallSpeed;
	}

	public void StopBall()
	{
		ballMovementActive = false;
		if (ballRigidbody != null)
		{
			ballRigidbody.linearVelocity = Vector2.zero;
			ballRigidbody.angularVelocity = 0f;
		}
	}

	public Rigidbody2D GetBallRigidbody()
	{
		return ballRigidbody;
	}

	public IEnumerator MovementDelayCoroutine(Vector2 direction, float delay)
	{
		yield return new WaitForSeconds(delay);

		if (ballRigidbody == null)
			yield break;

		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0f;

		ballRigidbody.position = spawnPosition;

		Physics2D.SyncTransforms();

		ballMovementActive = true;

		if (circleCollider != null)
			circleCollider.enabled = true;

		ResolveStartingOverlap();

		Vector2 spawnDirection = direction.sqrMagnitude > 0.0001f ? direction.normalized : lastValidDirection;

		ballRigidbody.linearVelocity = spawnDirection * Mathf.Clamp(currentSpeed, speed, maxSpeed);

		ClearGhosts();

		RecordTrailSample();

		ApplySpawnSpin();
		PlaySpawnSound();
	}

	public Vector2 GetSpawnPosition()
	{
		return spawnPosition;
	}
}
