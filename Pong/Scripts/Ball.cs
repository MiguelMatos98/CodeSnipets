using System.Collections;
using UnityEngine;

[RequireComponent(typeof(Rigidbody2D))]
[RequireComponent(typeof(CircleCollider2D))]
[RequireComponent(typeof(Trail))]
public class Ball : MonoBehaviour
{
	private int minSpeed;
	private int maxSpeed;
	private int speedGainAmount;
	
	private float verticalLimit;
	private float horizontalDeflectionAmount;
	private float spinStrength;
	private float exitDistance;

	private Vector2 spawnPosition;
	private Vector2 incomingVelocity;

	private Rigidbody2D ballRigidbody;
	private CircleCollider2D circleCollider;
	private Trail trail;

	private void Awake()
	{
		/*
	    Rigidbody2D has already been set in the Inspector to Kinematic, Gravity Scale to 0, Collision Detection to Continuous to 
	    avoid tunneling through the wall or the pads, Interpolation to Interpolate because movement needs visual smoothing
	    */

		minSpeed = 6;
		maxSpeed = 15;
		speedGainAmount = 1;

		verticalLimit = 4.98f;
		horizontalDeflectionAmount = 0.15f;
		spinStrength = 720.0f;
		exitDistance = 3.5f;

		// Using the ball's starting scene position as its spawn point
		spawnPosition = transform.position;

		incomingVelocity = Vector2.zero;

		ballRigidbody = GetComponent<Rigidbody2D>();
		circleCollider = GetComponent<CircleCollider2D>();
		trail = GetComponent<Trail>();

		// Passing max speed to set the trail's ghost count
		trail.Initialize(maxSpeed);
	}

	private void FixedUpdate()
	{
		if (ballRigidbody.bodyType != RigidbodyType2D.Dynamic)
			return;

		ClampSpeed();

		if (circleCollider.enabled)
			CorrectMovementAtWalls();

		// Storing the velocity before the next collision changes it
		incomingVelocity = ballRigidbody.linearVelocity;

		trail.Record(ballRigidbody.position);

		// Keeping the spin constant against damping and friction
		ballRigidbody.angularVelocity = Mathf.Sign(ballRigidbody.angularVelocity) * spinStrength;

		trail.Refresh(ballRigidbody.linearVelocity.magnitude);
	}

	private void OnCollisionEnter2D(Collision2D collision)
	{
		if (!circleCollider.enabled || collision.contactCount == 0)
			return;

		ContactPoint2D contact = collision.GetContact(0);

		// Ignoring contacts where the ball is already moving away from the surface
		if (Vector2.Dot(incomingVelocity, contact.normal) >= 0.0f)
			return;

		ResolveBounce(contact.point, contact.normal, collision.collider);
	}

	public void Spawn()
	{
		gameObject.SetActive(true);

		ballRigidbody.simulated = true;
		ballRigidbody.position = spawnPosition;
		transform.position = spawnPosition;

		// Syncing transforms so the collider moves to the spawn position immediately
		Physics2D.SyncTransforms();

		trail.Clear();
		trail.ResetFade();

		ballRigidbody.WakeUp();
	}

	public void Launch(Vector2 direction)
	{
		circleCollider.enabled = true;
		ballRigidbody.bodyType = RigidbodyType2D.Dynamic;

		Vector2 launchDirection = direction.normalized;
		float launchSpeed = Mathf.Clamp(incomingVelocity.magnitude, minSpeed, maxSpeed);

		ballRigidbody.linearVelocity = launchDirection * launchSpeed;

		incomingVelocity = ballRigidbody.linearVelocity;

		// Spinning clockwise when moving right and counter clockwise when moving left
		ballRigidbody.angularVelocity = launchDirection.x > 0.0f ? -spinStrength : spinStrength;

		trail.Record(ballRigidbody.position);

		SFXManager.Instance.PlaySoundEffect(SFXType.BallRespawn);
	}

	public void Despawn()
	{
		ballRigidbody.linearVelocity = Vector2.zero;
		ballRigidbody.angularVelocity = 0.0f;

		incomingVelocity = Vector2.zero;

		circleCollider.enabled = false;
		ballRigidbody.bodyType = RigidbodyType2D.Kinematic;

		gameObject.SetActive(false);

		trail.Hide();
	}

	private void ReflectOffWall(Vector2 normal)
	{
		Vector2 incomingDirection = incomingVelocity.normalized;

		Vector2 reflectedDirection = Vector2.Reflect(incomingDirection, normal).normalized;

		// Preventing near vertical reflections off the top and bottom walls by enforcing a minimum horizontal direction
		if (Mathf.Abs(normal.y) > 0.99f && Mathf.Abs(reflectedDirection.x) < horizontalDeflectionAmount)
		{
			float sign = reflectedDirection.x != 0.0f ? Mathf.Sign(reflectedDirection.x) : (Random.value < 0.5f ? -1.0f : 1.0f);

			reflectedDirection.x += sign * horizontalDeflectionAmount;

			reflectedDirection.Normalize();
		}

		float reflectedSpeed = Mathf.Clamp(incomingVelocity.magnitude, minSpeed, maxSpeed);

		ballRigidbody.linearVelocity = reflectedDirection * reflectedSpeed;
	}
	
	private void CorrectMovementAtWalls()
	{
		// Converting the collider's radius from local-space to world-space units
		float worldRadius = circleCollider.radius * transform.lossyScale.x;

		Vector2 correctedPosition = ballRigidbody.position;
		Vector2 correctedVelocity = ballRigidbody.linearVelocity;

		// Correcting the ball's position if it exceeds the upper wall 
		if (correctedPosition.y + worldRadius > verticalLimit)
		{
			correctedPosition.y = verticalLimit - worldRadius;

			// Flipping the ball's direction from up to down
			if (correctedVelocity.y > 0.0f)
				correctedVelocity.y = -correctedVelocity.y;
		}
		// Correcting the ball's position if it exceeds the lower wall
		else if (correctedPosition.y - worldRadius < -verticalLimit)
		{
			correctedPosition.y = -verticalLimit + worldRadius;

			// Flipping the ball's direction from down to up
			if (correctedVelocity.y < 0.0f)
				correctedVelocity.y = -correctedVelocity.y;
		}
		// Returning if neither walls have been crossed
		else
			return;

		// Applying the corrected position and velocity to the Rigidbody
		ballRigidbody.position = correctedPosition;
		ballRigidbody.linearVelocity = correctedVelocity;
	}

	private Vector2 ReflectOffPad(Vector2 contactPoint, Collider2D padCollider)
	{
		Vector2 padCenter = padCollider.bounds.center;

		return (contactPoint - padCenter).normalized;
	}

	private void ResolveBounce(Vector2 point, Vector2 normal, Collider2D collider)
	{
		if (collider.CompareTag("Pad"))
		{
			if (!collider.TryGetComponent(out Pad pad))
				return;

			Vector2 reflectedDirection = ReflectOffPad(point, collider);

			float reflectedSpeed = Mathf.Clamp(incomingVelocity.magnitude + speedGainAmount, minSpeed, maxSpeed);

			ballRigidbody.linearVelocity = reflectedDirection * reflectedSpeed;

			ApplySpin(pad);
		}
		else
			ReflectOffWall(normal);

		// Store the outgoing velocity for subsequent collision handling
		incomingVelocity = ballRigidbody.linearVelocity;

		SFXManager.Instance.PlaySoundEffect(SFXType.BallBounce);
	}

	private void ClampSpeed()
	{
		Vector2 currentVelocity = ballRigidbody.linearVelocity;

		float currentSpeed = currentVelocity.magnitude;

		// Using the stored velocity if the current velocity is nearly zero
		if (currentSpeed <= 0.01f)
		{
			currentVelocity = incomingVelocity;
			currentSpeed = currentVelocity.magnitude;

			// Returning if the stored velocity is effectively zero
			if (currentSpeed <= 0.0001f)
				return;
		}

		float clampedSpeed = Mathf.Clamp(currentSpeed, minSpeed, maxSpeed);

		ballRigidbody.linearVelocity = currentVelocity.normalized * clampedSpeed;
	}

	public IEnumerator ExitGoalAndFadeTrail()
	{
		ClampSpeed();

		circleCollider.enabled = false;

		float exitSpeed = ballRigidbody.linearVelocity.magnitude;

		if (exitSpeed <= 0.0001f)
			yield break;

		float exitDuration = exitDistance / exitSpeed;

		yield return trail.FadeOut(exitDuration, exitSpeed);
	}

	// Correct Method name
	private void ApplySpin(Pad pad)
	{
		float spinDirection = 0.0f;

		if (pad.CurrentVelocity.y != 0.0f)
			// Moving up keeps the pad side's direction, moving down inverts it
			spinDirection = (pad.transform.position.x > 0.0f) == (pad.CurrentVelocity.y > 0.0f) ? 1.0f : -1.0f;
		else
			// Spin follows the ball's horizontal movement
			spinDirection = ballRigidbody.linearVelocity.x > 0.0f ? -1.0f : 1.0f;

		ballRigidbody.angularVelocity = spinDirection * spinStrength;
	}
}
