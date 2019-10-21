import java.io.*;
import java.security.*;
import java.util.*;

public class WorldCupLineupVis {

private static final int TIME_LIMIT = 10000;

public String checkData(String s) { return ""; }

SecureRandom r;
double injuryRate;
int[] atk = new int[30];
int[] def = new int[30];
int[] agg = new int[30];
int numGroups;
int[][] groupMembers;
int[] groupAtk;
int[] groupDef;
int[] groupAgg;
String[] players = new String[30];
String[] groups;

boolean[] done = new boolean[30];
boolean[] carded = new boolean[30];
int[] lineup = new int[10];
char[] position = new char[10];
int[][] order = new int[10][];
int[] calcAtk = new int[10];
int[] calcDef = new int[10];
int[] calcAgg = new int[10];

private void generateTestCase(long seed) {
  try {
    r = SecureRandom.getInstance("SHA1PRNG");
    r.setSeed(seed);
  } catch (NoSuchAlgorithmException e) {}
  injuryRate = r.nextDouble() * 0.05;
  for (int i = 0; i < 30; i++) {
    atk[i] = r.nextInt(100);
    def[i] = r.nextInt(100);
    agg[i] = r.nextInt(50);
    players[i] = atk[i] + "," + def[i] + "," + agg[i];
  }
  numGroups = r.nextInt(51);
  groupMembers = new int[numGroups][];
  groups = new String[numGroups];
  groupAtk = new int[numGroups];
  groupDef = new int[numGroups];
  groupAgg = new int[numGroups];
  ArrayList<Integer> currentGroup;
  for (int i = 0; i < numGroups; i++) {
    groupAtk[i] = r.nextInt(51) - 25;
    groupDef[i] = r.nextInt(51) - 25;
    groupAgg[i] = r.nextInt(51) - 25;
    currentGroup = new ArrayList<Integer>();
    int member1 = r.nextInt(30);
    int member2 = r.nextInt(29);
    if (member2 >= member1) member2++;
    currentGroup.add(member1);
    currentGroup.add(member2);
    while (r.nextDouble() < 0.5) {
      int testMember = r.nextInt(30);
      if (currentGroup.contains(testMember)) break;
      currentGroup.add(testMember);
    }
    Collections.sort(currentGroup);
    groupMembers[i] = new int[currentGroup.size()];
    for (int j = 0; j < groupMembers[i].length; j++) groupMembers[i][j] = currentGroup.get(j);
    groups[i] = "" + groupMembers[i][0];
    for (int j = 1; j < groupMembers[i].length; j++) groups[i] += "," + groupMembers[i][j];
    groups[i] += " " + groupAtk[i] + "," + groupDef[i] + "," + groupAgg[i];
  }
}

public String displayTestCase(String seed) {
  long s = Long.parseLong(seed);
  generateTestCase(s);
  StringBuilder sb = new StringBuilder();
  sb.append("Players:\n");
  for (int i = 0; i < 30; i++) sb.append(players[i] + "\n");
  sb.append("\nGroups:\n");
  for (int i = 0; i < numGroups; i++) sb.append(groups[i] + "\n");
  return sb.toString();
}

private void writeError(String s) {
  if (!vis) return;
  System.out.println(s);
}

private boolean groupPresent(int g) {
  for (int i = 0; i < groupMembers[g].length; i++) {
    boolean found = false;
    for (int j = 0; j < 10; j++) {
      if (groupMembers[g][i] == lineup[j]) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  return true;
}

private int getPosition(int p) {
  for (int i = 0; i < 10; i++) {
    if (lineup[i] == p) return i;
  }
  return -1;
}

private int createCurrentLineup() {
  for (int i = 0; i < 10; i++) {
    if (lineup[i] > -1) continue;
    for (int j = 0; j < order[i].length; j++) {
      if (!done[order[i][j]]) {
        lineup[i] = order[i][j];
        done[lineup[i]] = true;
        writeError("Position " + i + " (" + position[i] + ") is being filled by player " + lineup[i]);
        break;
      }
    }
  }
  
  for (int i = 0; i < 10; i++) {
    calcAtk[i] = calcDef[i] = calcAgg[i] = 0;
    if (lineup[i] == -1) continue;
    calcAgg[i] = agg[lineup[i]];
    if (position[i] == 'F') {
      calcAtk[i] = atk[lineup[i]] * 2;
    }
    if (position[i] == 'D') {
      calcDef[i] = def[lineup[i]] * 2;
    }
    if (position[i] == 'M') {
      calcAtk[i] = atk[lineup[i]];
      calcDef[i] = def[lineup[i]];
    }
  }
  
  // Add group bonuses
  for (int i = 0; i < numGroups; i++) {
    if (!groupPresent(i)) continue;
    for (int j = 0; j < groupMembers[i].length; j++) {
      int p = getPosition(groupMembers[i][j]);
      calcAgg[p] += groupAgg[i];
      if (position[p] == 'F') {
        calcAtk[p] += groupAtk[i] * 2;
      }
      if (position[p] == 'D') {
        calcDef[p] += groupDef[i] * 2;
      }
      if (position[p] == 'M') {
        calcAtk[p] += groupAtk[i];
        calcDef[p] += groupDef[i];
      }
    }
  }
  
  int totalAtk = 0;
  int totalDef = 0;
  writeError("Current lineup:");
  for (int i = 0; i < 10; i++) {
    totalAtk += calcAtk[i];
    totalDef += calcDef[i];
    writeError("Position " + i + " (" + position[i] + "): " + lineup[i] + ": Atk = " + calcAtk[i] + ", Def = " + calcDef[i] + ", Agg = " + calcAgg[i]);
  }
  writeError("Total attack = " + totalAtk + ", Total defense = " + totalDef + "\n");
  return Math.min(totalAtk, totalDef);
}

private void processLineup() {
  for (int i = 0; i < 10; i++) {
    if (lineup[i] == -1) continue;
    if (r.nextDouble() < injuryRate) {
      writeError("Player " + lineup[i] + " in position " + i + " was injured.");
      lineup[i] = -1;
      continue;
    }
    if (r.nextInt(100) < calcAgg[i]) {
      if (carded[lineup[i]]) {
        writeError("Player " + lineup[i] + " in position " + i + " was shown the red card, and ejected.");
        lineup[i] = -1;
      } else {
        writeError("Player " + lineup[i] + " in position " + i + " was shown the yellow card, but is still in the game.");
        carded[lineup[i]] = true;
      }
    }
  }
  writeError("");
}

// ------------- visualization part ------------
static String exec;
static boolean vis, debug;
static Process proc;
InputStream is;
OutputStream os;
BufferedReader br;

private String[] selectPositions(String[] players, String[] groups) throws IOException {
  StringBuffer sb = new StringBuffer();
  sb.append(players.length).append("\n");
  for (int i = 0; i < players.length; ++i) sb.append(players[i]).append("\n");
  sb.append(groups.length).append("\n");
  for (int i = 0; i < groups.length; ++i) sb.append(groups[i]).append("\n");
  os.write(sb.toString().getBytes());
  os.flush();
  String[] ret = new String[Integer.parseInt(br.readLine())];
  for (int i = 0; i < ret.length; i++) ret[i] = br.readLine();
  return ret;
}

public double runTest(String testValue) {
  writeError(displayTestCase(testValue));
  String[] ret;
  try {
    ret = selectPositions(players, groups);
  } catch (Exception e) {
    writeError("Error calling selectPositions()");
    return -1;
  }

  if (ret.length != 10) {
    writeError("Return length must be exactly 10 elements.");
    return -1;
  }
  
  // Parse return value
  for (int i = 0; i < 10; i++) {
    String s = ret[i];
    if (s.length() == 0) {
      writeError("Element " + i + " of the return is invalid.");
      return -1;
    }
    if (s.charAt(0) != 'F' && s.charAt(0) != 'M' && s.charAt(0) != 'D') {
      writeError("Element " + i + " of the return does not start with a valid position (F,M,D).");
      return -1;
    }
    position[i] = s.charAt(0);
    if (s.length() < 3) {
      order[i] = new int[0];
      continue;
    }
    if (s.charAt(1) != ' ') {
      writeError("Element " + i + " of the return must be a position followed by a space.");
      return -1;
    }
    String[] t = s.substring(2).split(",");
    order[i] = new int[t.length];
    for (int j = 0; j < t.length; j++) {
      try {
        order[i][j] = Integer.parseInt(t[j]);
      } catch (NumberFormatException e) {
        writeError("Unable to parse value " + t[j] + " in return element " + i);
        return -1;
      }
      if (order[i][j] < 0 || order[i][j] > 29) {
        writeError("Invalid player number " + order[i][j] + " in return element " + i);
        return -1;
      }
    }
  }
  for (int i = 0; i < 10; i++) lineup[i] = -1;
  
  int totalScore = 0;
  writeError("\nBeginning of game...");
  writeError("At the start of the game...");
  totalScore += createCurrentLineup();
  processLineup();
  writeError("Midway through the first half...");
  totalScore += createCurrentLineup();
  processLineup();
  writeError("At the start of the second half...");
  totalScore += createCurrentLineup();
  processLineup();
  writeError("Midway through the second half...");
  totalScore += createCurrentLineup();
  processLineup();
  writeError("Wrapping up the second half...");
  totalScore += createCurrentLineup();
  return Math.max(0, totalScore);
}

    public WorldCupLineupVis(String seed) {
      try {
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                new ErrorReader(proc.getErrorStream()).start();
            } catch (Exception e) { e.printStackTrace(); }
        }
        System.out.println("Score = " + runTest(seed));
        if (proc != null)
            try { proc.destroy(); } 
            catch (Exception e) { e.printStackTrace(); }
      }
      catch (Exception e) { e.printStackTrace(); }
    }
    // -----------------------------------------
    public static void main(String[] args) {
        String seed = "1";
        vis = true;
        for (int i = 0; i<args.length; i++)
        {   if (args[i].equals("-seed"))
                seed = args[++i];
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-novis"))
                vis = false;
            if (args[i].equals("-debug"))
                debug = true;
        }

        WorldCupLineupVis f = new WorldCupLineupVis(seed);
    }
}

class ErrorReader extends Thread{
    InputStream error;
    public ErrorReader(InputStream is) {
        error = is;
    }
    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0)
            {   String s = new String(ch,0,read);
                System.out.print(s);
                System.out.flush();
            }
        } catch(Exception e) { }
    }
}

