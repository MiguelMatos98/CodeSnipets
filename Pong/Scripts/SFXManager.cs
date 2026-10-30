using System.Collections.Generic;
using UnityEngine;

public enum SFXType
{
	BallBounce = 0,
	Score = 1,
	BallRespawn = 2
}

[System.Serializable]
public class SoundEffect
{
	public SFXType type;
	
	public AudioClip clip;

	[Range(0f, 1f)] public float volume = 1f;

	[HideInInspector] public AudioSource source;
}

public class SFXManager : MonoBehaviour
{
	public static SFXManager Instance { get; private set; }

	[SerializeField] private List<SoundEffect> soundEffects = new List<SoundEffect>();

	private void Awake()
	{
		// Virify that there only exists one instance of SFXManager
		if (Instance != null && Instance != this)
		{
			// In case wheter there already exists another instance of SFX manager
			// Destroy that other instace and return here to prevent any further new initialization of this class
			Destroy(gameObject);

			return;
		}

		// Otherwise (If no other instance of SFX manager exists besides this one),
		// store the only existing instance of this script in memory inside SFXManager's Instance variable (So it can be mutatable)
		Instance = this;

		// Generate an AudioSource for each sound effect on startup
		foreach (SoundEffect sfx in soundEffects)
		{
			sfx.source = gameObject.AddComponent<AudioSource>();
			sfx.source.clip = sfx.clip;
			sfx.source.volume = sfx.volume;
			sfx.source.playOnAwake = false;
		}
	}

	public void PlaySoundEffect(SFXType soundEffectType)
	{
		// Cast the parameter ssound efftc type to an int
		// (Note: Retaining your original variable here, even though the new method searches by the enum type directly!)
		int soundEffectIndex = (int)soundEffectType;

		// Play audio source from a list of audio clips by retiving it's audio clip based on the sound effect index
		SoundEffect sfxToPlay = soundEffects.Find(sfx => sfx.type == soundEffectType);

		if (sfxToPlay != null)
		{
			if (sfxToPlay.source == null || sfxToPlay.clip == null)
				return;

			sfxToPlay.source.pitch = 1f;

			double dspStartTime = AudioSettings.dspTime;

			sfxToPlay.source.PlayScheduled(dspStartTime + 0.1);
		}
	}
}
