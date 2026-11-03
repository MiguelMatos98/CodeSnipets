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
	private float deflectionAmount;
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
		deflectionAmount = 0.2f;
		spinStrength = 720.0f;
		exitDistance = 3.5f;

		// Storing the ball's starting scene position as its spawn point
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
		// Ignoring ball movement when its rigibody is kinematic
		if (ballRigidbody.bodyType == RigidbodyType2D.Kinematic)
			return;

		// Correcting the ball's movement when it's within limits
		if (circleCollider.enabled)
			KeepWithinLimits();

		// Storing the incoming velocity for future bounces calculations
		incomingVelocity = ballRigidbody.linearVelocity;

		//      DON'T COMMENT YET, ADDRESS SCRIPT FIRST 
		// ---------------------------------------------------
		trail.Record(ballRigidbody.position);
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

		ApplyBounce(contact, collision.collider);
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

	private void KeepWithinLimits()
	{
		// Scaling the collider's radius from local-space to world-space units
		float worldRadius = circleCollider.radius * transform.lossyScale.x;

		Vector2 correctedPosition = ballRigidbody.position;
		Vector2 correctedVelocity = ballRigidbody.linearVelocity;

		// Correcting the ball's position when exceeds the upper wall 
		if (correctedPosition.y + worldRadius > verticalLimit)
		{
			correctedPosition.y = verticalLimit - worldRadius;

			// Flipping the ball's direction away from the upper wall (up to down)
			if (correctedVelocity.y > 0.0f)
				correctedVelocity.y = -correctedVelocity.y;
		}
		// Correcting the ball's position when exceeds the lower wall
		else if (correctedPosition.y - worldRadius < -verticalLimit)
		{
			correctedPosition.y = -verticalLimit + worldRadius;

			// Flipping the ball's direction away from the lower wall (down to up)
			if (correctedVelocity.y < 0.0f)
				correctedVelocity.y = -correctedVelocity.y;
		}
		// Returning when neither walls have been crossed
		else
			return;

		// Applying the newly corrected position and velocity to the Rigidbody
		ballRigidbody.position = correctedPosition;
		ballRigidbody.linearVelocity = correctedVelocity;
	}

	private Vector2 EnsureMinimumDeflection(Vector2 direction)
	{
		// Preventing near-vertical trajectories by enforcing a minimum horizontal direction
		if (Mathf.Abs(direction.x) < deflectionAmount)
		{
			float sign = direction.x != 0.0f ? Mathf.Sign(direction.x) : (Random.value < 0.5f ? -1.0f : 1.0f);

			direction.x = sign * deflectionAmount;
			direction.y = Mathf.Sign(direction.y) * Mathf.Sqrt(1.0f - deflectionAmount * deflectionAmount);
		}
		// Preventing near-horizontal trajectories by enforcing a minimum vertical direction
		else if (Mathf.Abs(direction.y) < deflectionAmount)
		{
			float sign = direction.y != 0.0f ? Mathf.Sign(direction.y) : (Random.value < 0.5f ? -1.0f : 1.0f);

			direction.y = sign * deflectionAmount;
			direction.x = Mathf.Sign(direction.x) * Mathf.Sqrt(1.0f - deflectionAmount * deflectionAmount);
		}

		return direction;
	}

	private Vector2 BounceOffWall(ContactPoint2D contact)
	{
		Vector2 incomingDirection = incomingVelocity.normalized;

		return Vector2.Reflect(incomingDirection, contact.normal).normalized;
	}

	private Vector2 BounceOffPad(ContactPoint2D contact, Collider2D padCollider)
	{
		Vector2 padCenter = padCollider.bounds.center;

		Vector2 hitOffset = contact.point - padCenter;

		Vector2 padExtents = padCollider.bounds.extents;

		// Front and back faces steer vertically by hit height.
		if (Mathf.Abs(contact.normal.x) > Mathf.Abs(contact.normal.y))
		{
			float verticalSteer = Mathf.Clamp(hitOffset.y / padExtents.y, -1.0f, 1.0f);

			return new Vector2(Mathf.Sign(contact.normal.x), verticalSteer).normalized;
		}

		// Upper and lower faces steer horizontally by hit position.
		float horizontalSteer = Mathf.Clamp(hitOffset.x / padExtents.x, -1.0f, 1.0f);

		return new Vector2(horizontalSteer, Mathf.Sign(contact.normal.y)).normalized;
	}

	private void ApplyBounce(ContactPoint2D contact, Collider2D collider)
	{
		Vector2 incomingDirection = incomingVelocity.normalized;

		Pad pad = null;

		Vector2 reflectedDirection;

		float reflectedSpeed;

		if (collider.CompareTag("Pad"))
		{
			if (!collider.TryGetComponent(out pad))
				return;

			reflectedDirection = BounceOffPad(contact, collider);

			reflectedSpeed = Mathf.Clamp(incomingVelocity.magnitude + speedGainAmount, minSpeed, maxSpeed);
		}
		else
		{
			reflectedDirection = BounceOffWall(contact);

			reflectedSpeed = Mathf.Clamp(incomingVelocity.magnitude, minSpeed, maxSpeed);
		}

		Vector2 correctedDirection = EnsureMinimumDeflection(reflectedDirection);

		Vector2 uncorrectedVelocity = reflectedDirection * reflectedSpeed;
		Vector2 correctedVelocity = correctedDirection * reflectedSpeed;

		float directionChange = Vector2.Angle(reflectedDirection, correctedDirection);

		// Debugging trajectory corrections
		if (directionChange > 0.001f)
		{
			Debug.Log($"Trajectory corrected | Before: ({uncorrectedVelocity.x:F4}, {uncorrectedVelocity.y:F4}) | " + $"After: ({correctedVelocity.x:F4}, {correctedVelocity.y:F4}) | " + $"Direction change: {directionChange:F4} degrees | " + $"Speed: {correctedVelocity.magnitude:F2}", this);

			Debug.DrawRay(contact.point, reflectedDirection * 2.0f, Color.cyan, 2.0f);
			Debug.DrawRay(contact.point, correctedDirection * 2.0f, Color.magenta, 2.0f);
		}

		ballRigidbody.linearVelocity = correctedVelocity;

		if (pad != null)
			ApplySpin(pad);

		Vector2 outgoingVelocity = ballRigidbody.linearVelocity;

		// Debugging incoming and outgoing bounce directions
		string bounceType = collider.CompareTag("Pad") ? "Pad" : "Wall";

		Debug.Log($"{bounceType} bounce | Incoming: ({incomingVelocity.x:F4}, {incomingVelocity.y:F4}) | " + $"Outgoing: ({outgoingVelocity.x:F4}, {outgoingVelocity.y:F4}) | " + $"Speed: {outgoingVelocity.magnitude:F2}", this);

		Debug.DrawRay(contact.point, incomingDirection * 2.0f, Color.yellow, 2.0f);
		Debug.DrawRay(contact.point, outgoingVelocity.normalized * 2.0f, Color.green, 2.0f);

		// Storing the outgoing velocity for subsequent collision handling
		incomingVelocity = outgoingVelocity;

		SFXManager.Instance.PlaySoundEffect(SFXType.BallBounce);
		SFXManager.Instance.PlaySoundEffect(SFXType.BallBounce);
	}

	private Vector2 GetClampedVelocity(Vector2 currentVelocity)
	{
		// Retriving the ball's speed from linear velocty's magnitude
		float currentSpeed = currentVelocity.magnitude;

		// Clamping the current speed so it remains within the minimum and maximum speed values
		float clampedSpeed = Mathf.Clamp(currentSpeed, minSpeed, maxSpeed);

		// Returning a new velocity by multiplying its normalised direction by the clamped speed
		return currentVelocity.normalized * clampedSpeed;
	}

	public IEnumerator ExitGoal()
	{
		Vector2 clampedVelocity = GetClampedVelocity(ballRigidbody.linearVelocity);

		ballRigidbody.linearVelocity = clampedVelocity;

		circleCollider.enabled = false;

		float exitSpeed = clampedVelocity.magnitude;

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
