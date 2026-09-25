using UnityEngine;

[RequireComponent(typeof(Rigidbody2D))]
[RequireComponent(typeof(Collider2D))]
public class Pad : MonoBehaviour
{
	private const string LeftVerticalAxis = "Vertical_1";
	private const string RightVerticalAxis = "Vertical_2";

	[SerializeField] private float speed = 10f;
	[SerializeField] private float verticalLimit = 4.85f;

	private float padHalfHeight;
	private float maxAllowedY;
	private float minAllowedY;
	private float verticalInput;

	private string inputAxisName;

	private Rigidbody2D padRigidbody;
	private Collider2D padCollider;

	public Vector2 CurrentVelocity { get; private set; }

	private void Awake()
	{
		padRigidbody = GetComponent<Rigidbody2D>();
		padCollider = GetComponent<Collider2D>();

		padHalfHeight = padCollider.bounds.extents.y;

		// Adjusting the vertical movement limits to the pad's half-height to keep its edges within bounds
		maxAllowedY = verticalLimit - padHalfHeight;
		minAllowedY = -verticalLimit + padHalfHeight;

		// Setting the left pad to use the Vertical_1 axis and the right pad to use the Vertical_2 axis
		inputAxisName = transform.position.x < 0f ? LeftVerticalAxis : RightVerticalAxis;

		// Configuring the Rigidbody2D for dynamic physics interactions, continuous collision detection and no gravity
		padRigidbody.bodyType = RigidbodyType2D.Dynamic;
		padRigidbody.collisionDetectionMode = CollisionDetectionMode2D.Continuous;
		padRigidbody.gravityScale = 0f;

		// Disabling interpolation and extrapolation since the Rigidbody2D's movement causes no visible jitter
		padRigidbody.interpolation = RigidbodyInterpolation2D.None;

		// Locking the X axis and rotation for vertical movement only
		padRigidbody.constraints = RigidbodyConstraints2D.FreezePositionX | RigidbodyConstraints2D.FreezeRotation;
	}

	private void Update()
	{
		// Reading player input every frame before applying Rigidbody2D movement in FixedUpdate
		verticalInput = Input.GetAxisRaw(inputAxisName);
	}

	private void FixedUpdate()
	{
		float currentY = padRigidbody.position.y;
		float targetVelocityY = verticalInput * speed;

		// Predicting the next position to prevent the pad from overshooting
		float predictedY = currentY + targetVelocityY * Time.fixedDeltaTime;

		// Keeping the predicted position within the movement limits
		float clampedY = Mathf.Clamp(predictedY, minAllowedY, maxAllowedY);

		// Determining the velocity needed to reach the clamped position during this physics update
		targetVelocityY = (clampedY - currentY) / Time.fixedDeltaTime;

		padRigidbody.linearVelocity = new Vector2(0f, targetVelocityY);

		CurrentVelocity = padRigidbody.linearVelocity;
	}

	public float GetSpeed()
	{
		return speed;
	}

	public float GetVerticalLimit()
	{
		return verticalLimit;
	}

	public Vector2 GetMoveDirection()
	{
		return new Vector2(0f, verticalInput);
	}
}
