using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CollectableItem : MonoBehaviour
{
    public enum ItemType
    {
        Apple,
        Orange, 
        Coin
	}
    public ItemType itemType;
}
