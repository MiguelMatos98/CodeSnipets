using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SFXManager : MonoBehaviour
{
    public enum SoundEffectLib
    {
        Bounce,
        Goal,
        Respawn
    }

    public List<SoundEffectLib> SFX_List = new List<SoundEffectLib>();
    public List<AudioClip> AudioClip_List = new List<AudioClip>();

    public Dictionary<SoundEffectLib, AudioClip> SFX_Lib = 
        new Dictionary<SoundEffectLib, AudioClip>();

    public static SFXManager TheSFXManager;

    public GameObject SFXOBJ;

    void Start()
    {
        TheSFXManager = this;

        for (int i = 0; i < SFX_List.Count; i++)
        {
            SFX_Lib.Add(SFX_List[i], AudioClip_List[i]);
        }
    }

    public void PlaySFX(SoundEffectLib SFXToPlay)
    {
        if(SFX_Lib.ContainsKey(SFXToPlay))
        {
            GameObject GO = Instantiate(SFXOBJ);
            GO.GetComponent<AudioSource>().PlayOneShot(SFX_Lib[SFXToPlay]);
            Destroy(GO, SFX_Lib[SFXToPlay].length);
        }
    }

}
