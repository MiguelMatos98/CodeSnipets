using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ScoreManager : MonoBehaviour
{
    public int score;

    public int ufoPoints;

    public int asteroidPoints;

    public Text scoreText;

    public enum ScoreType
    {
        UFO,
        Asteroid
	}

	public static ScoreManager instance;

	void Start()
    {
        instance = this;
	}

    public void UpdateScore(ScoreType scoreType)
    {
       switch(scoreType)
       {
         case ScoreType.UFO:
             score += ufoPoints;
             break;
         case ScoreType.Asteroid:
             score += asteroidPoints;
             break;
		}
        scoreText.text = "Score: " + score.ToString("0");
	}
	void Update()
    {
        
    }
}
