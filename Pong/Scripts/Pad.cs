using UnityEngine;

[RequireComponent(typeof(Rigidbody2D))]
[RequireComponent(typeof(Collider2D))]
public class Pad : MonoBehaviour
{
	private const string LeftVerticalAxis = "Vertical_1";
	private const string RightVerticalAxis = "Vertical_2";

	private float speed = 10f;
	private float verticalLimit = 4.85f;

	private float halfHeight;
	private float maxAllowedY;
	private float minAllowedY;
	private float verticalInput;

	private string inputAxisName;

	private Rigidbody2D padRigidbody;
	private Collider2D padCollider;

	public Vector2 CurrentVelocity { get; private set; }

	private void Awake()
	{
	    /*
	    Rigidbody2D has already been set in the Inspector to Dynamic, Gravity Scale to 0, Collision Detection to Continuous to 
	    avoid tunneling through the ball, Interpolation to None because movement needs no visual smoothing, and Constraints to 
	    freeze X position and Z rotation for moving only vertically
		*/

		padRigidbody = GetComponent<Rigidbody2D>();
		padCollider = GetComponent<Collider2D>();

		// Storing halfheigh from pad's collider bounds to use for movement limit calculations
		halfHeight = padCollider.bounds.extents.y;

		// Adjusting the vertical movement limits to the pad's half-height to keep its edges within bounds
		maxAllowedY = verticalLimit - halfHeight;
		minAllowedY = -verticalLimit + halfHeight;

		// Setting the left pad to use the Vertical_1 axis and the right pad to use the Vertical_2 axis
		inputAxisName = transform.position.x < 0.0f ? LeftVerticalAxis : RightVerticalAxis;
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
