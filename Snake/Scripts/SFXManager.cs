using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SFXManager : MonoBehaviour
{
	public static SFXManager Instance;

	public enum Sounds
	{
		Lose,
		Eat,
	}

	[System.Serializable]
	public struct SFX
	{
		public Sounds soundEffects;
		public AudioClip audioClip;
	}
	public List<SFX> SFXList = new List<SFX>();


	public GameObject SFXPrefab;
	public Dictionary<Sounds, AudioClip> SFXDictionary = new Dictionary<Sounds, AudioClip>();
	public AudioSource audioSource = new AudioSource();

	void Start()
	{
		Instance = this;

		for (int i = 0; i < SFXList.Count; i++)
		{
			SFXDictionary.Add(SFXList[i].soundEffects, SFXList[i].audioClip);
		}
	}

	public void PlaySFX(Sounds soundToPlay)
	{
		if (SFXDictionary.ContainsKey(soundToPlay))
		{
			GameObject sfxObject = Instantiate(SFXPrefab);
			sfxObject.GetComponent<AudioSource>().PlayOneShot(SFXDictionary[soundToPlay]);
			Destroy(sfxObject, SFXDictionary[soundToPlay].length);
		}
	}
}

