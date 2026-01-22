using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderPlacer : MonoBehaviour
{

    [SerializeField] private Transform topCollider;
	[SerializeField] private Transform bottumCollider;
	[SerializeField] private Transform rightCollider;
	[SerializeField] private Transform leftCollider;

	void Start()
    {
        topCollider.position = new Vector3(0, SnakeHead.halfScreen.y, 0);

        bottumCollider.position = new Vector3(0, -SnakeHead.halfScreen.y, 0);

        rightCollider.position = new Vector3(SnakeHead.halfScreen.x, 0, 0);

        leftCollider.position = new Vector3(-SnakeHead.halfScreen.x, 0, 0);
	}
}
