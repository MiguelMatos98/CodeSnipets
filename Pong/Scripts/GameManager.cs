using System;
using System.Collections;
using UnityEngine;
using TMPro;

public class GameManager : MonoBehaviour
{
	public static GameManager Instance { get; private set; }

	public event Action OnGameStarted;

	[Header("References")]
	[SerializeField] private Ball ball;
	[SerializeField] private TextMeshProUGUI player1ScoreText;
	[SerializeField] private TextMeshProUGUI player2ScoreText;

	[Header("Game Settings")]
	[SerializeField] private int winningScore = 9;
	[SerializeField] private float roundDelayTime  = 1.0f;

	private int player1Score = 0;
	private int player2Score = 0;

	private bool isGameOver = false;
	private bool hasScored = true;
	private bool hasGameStarted = false;

	private Coroutine roundCoroutine;
	private void Awake()
	{
		if (Instance != null && Instance != this)
		{
			Destroy(gameObject);
			return;
		}

		Instance = this;
	}

	private void Start()
	{
		// Prevent the ball from spawning before Play is pressed
		if (ball != null)
			ball.Despawn();

		UpdateScoreUI();
	}

	private void ResetGame()
	{
		StopAllCoroutines();
		roundCoroutine = null;

		player1Score = 0;
		player2Score = 0;
		UpdateScoreUI();

		isGameOver = false;
		hasScored = false;

		ball.Despawn();
	}

	public void StartGame()
	{
		if (hasGameStarted)
			return;

		if (ball == null)
		{
			Debug.LogError("[GameManager] Ball reference is missing!");
			return;
		}

		ResetGame();
		hasGameStarted = true;

		StartRound(0.1f);
		OnGameStarted?.Invoke();
	}

	public void RestartGame()
	{
		if (ball == null)
			return;

		ResetGame();
		hasGameStarted = true;

		StartRound(0f);
		OnGameStarted?.Invoke();
	}

	private void StartRound(float delay)
	{
		if (roundCoroutine != null)
			StopCoroutine(roundCoroutine);

		roundCoroutine = StartCoroutine(RunRoundIntro(delay));
	}

	private IEnumerator RunRoundIntro(float delay)
	{
		hasScored = false;
		ball.Spawn();

		yield return new WaitForSeconds(delay);

		ball.Launch(PickRoundDirection());
	}

	public void AwardPointToPlayer1()
	{
		if (hasScored || isGameOver) return;
		hasScored = true;

		player1Score++;
		UpdateScoreUI();

		BeginPointSequence();
	}

	public void AwardPointToPlayer2()
	{
		if (hasScored || isGameOver) return;
		hasScored = true;

		player2Score++;
		UpdateScoreUI();

		BeginPointSequence();
	}

	private void BeginPointSequence()
	{
		if (SFXManager.Instance != null)
			SFXManager.Instance.PlaySoundEffect(SFXType.Score);

		StartCoroutine(RunPointSequence());
	}

	private IEnumerator RunPointSequence()
	{
		// Allow the ghost trail to fade before despawning the ball
		yield return ball.ExitGoalAndFadeTrail();

		ball.Despawn();

		if (IsGameWon())
		{
			EndGame();

			yield break;
		}

		yield return new WaitForSeconds(roundDelayTime);

		StartRound(0.0f);
	}

	private void UpdateScoreUI()
	{
		if (player1ScoreText != null)
			player1ScoreText.text = player1Score.ToString();

		if (player2ScoreText != null)
			player2ScoreText.text = player2Score.ToString();
	}

	private Vector2 PickRoundDirection()
	{
		float x = UnityEngine.Random.value < 0.5f ? -1f : 1f;
		float y = UnityEngine.Random.Range(-0.35f, 0.35f);
		return new Vector2(x, y).normalized;
	}

	private bool IsGameWon()
	{
		return player1Score >= winningScore || player2Score >= winningScore;
	}

	private void EndGame()
	{
		isGameOver = true;
		hasGameStarted = false;

		int winner = player1Score >= winningScore ? 1 : 2;

		if (UIManager.Instance != null)
			UIManager.Instance.DisplayNotification($"Player {winner} Wins!");
	}
}
