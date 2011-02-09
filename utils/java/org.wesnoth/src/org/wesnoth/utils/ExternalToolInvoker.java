/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import org.wesnoth.Constants;
import org.wesnoth.Logger;
import org.wesnoth.Messages;

/**
 * A tool to invoke an external tool/executable
 */
public class ExternalToolInvoker
{
	private Process process_;
	private ProcessBuilder processBuilder_;

	private BufferedReader bufferedReaderOutput_;
	private BufferedReader bufferedReaderError_;

	// Thread for monitoring stdout
	private Thread monitorOutputThread_;
	private StringBuilder outputContent_;

	// Thread for monitoring stderr
	private Thread monitorErrorThread_;
	private StringBuilder errorContent_;

	private List<String> arguments_;

	/**
	 * Creates an external tool invoker with specified options
	 *
	 * @param fileName the file name to be invoked
	 * @param arguments the arguments passed to the file
	 */
	public ExternalToolInvoker(String fileName, List<String> arguments) {
		List<String> commandline = new ArrayList<String>();
		commandline.add(fileName);
		if (arguments != null)
			commandline.addAll(arguments);

		arguments_ = commandline;

		processBuilder_ = new ProcessBuilder(commandline);
		Logger.getInstance().log(String.format(Messages.ExternalToolInvoker_0,
				fileName, arguments));

	 	outputContent_ = new StringBuilder();
	 	errorContent_ = new StringBuilder();
	}

	/**
	 * Runs the current tool
	 */
	public void runTool()
	{
		try
		{
			process_ = processBuilder_.start();
			Reader stdoutReader = null;
			Reader stderrReader = null;

			if (arguments_.get(0).toLowerCase(Locale.ENGLISH).contains("wesnoth.exe")  && //$NON-NLS-1$
				Constants.IS_WINDOWS_MACHINE)
			{
				String wesnothParent = new File(arguments_.get(0)).getParent() + "/"; //$NON-NLS-1$
				if (new File(wesnothParent + "stdout.txt").exists()) //$NON-NLS-1$
					stdoutReader = new FileReader(wesnothParent + "stdout.txt"); //$NON-NLS-1$
				if (new File(wesnothParent + "stderr.txt").exists()) //$NON-NLS-1$
					stderrReader = new FileReader(wesnothParent + "stderr.txt"); //$NON-NLS-1$
			}
			else
			{
				stdoutReader = new InputStreamReader(process_.getInputStream());
				stderrReader = new InputStreamReader(process_.getErrorStream());
			}

			if (stdoutReader != null)
				bufferedReaderOutput_ = new BufferedReader(stdoutReader);

			if (stderrReader != null)
				bufferedReaderError_ = new BufferedReader(stderrReader);
		} catch (IOException e)
		{
			Logger.getInstance().logException(e);
		}
	}

	/**
	 * Waits for the current tool, and returns the return value
	 *
	 * if the process is null (not started) => 0
	 * if there was an error => -1
	 *
	 * @return the return value of the tool
	 */
	public int waitForTool()
	{
		try
		{
			if (process_ == null)
				return 0;
			return process_.waitFor();
		} catch (InterruptedException e)
		{
			return -1;
		}
	}

	/**
	 * Reads a line from the stdout.
	 * Returns null if process wasn't started or an exception was thrown
	 *
	 * @return
	 */
	public String readOutputLine()
	{
		if (process_ == null || bufferedReaderOutput_ == null)
			return null;

		try
		{
			return bufferedReaderOutput_.readLine();
		} catch (IOException e)
		{
			Logger.getInstance().logException(e);
			return null;
		}
	}

	/**
	 * Reads a line from the stderr.
	 * Returns null if process wasn't started or an exception was thrown
	 *
	 * @return
	 */
	public String readErrorLine()
	{
		if (process_ == null || bufferedReaderError_ == null)
			return null;

		try
		{
			return bufferedReaderError_.readLine();
		} catch (IOException e)
		{
			Logger.getInstance().logException(e);
			return null;
		}
	}

	/**
	 * Starts a new thread monitoring stderr.
	 * All "Error" output will be available to be read from <code>getErrorContent()</code>
	 */
	public void startErrorMonitor()
	{
		startErrorMonitor(new OutputStream[0]);
	}

	/**
	 * Starts a new thread monitoring stderr.
	 * All "Error" output will be available to be read from <code>getErrorContent()</code>
	 * @param extraStreams The extra streams array where stderr will be written or null if none
	 */
	public void startErrorMonitor(final OutputStream[] extraStreams)
	{
		monitorErrorThread_ = new Thread(new Runnable() {
			@Override
			public void run()
			{
				try
				{
					String line = ""; //$NON-NLS-1$
					while((line = readErrorLine()) != null)
					{
						if (extraStreams != null)
						{
							for(OutputStream stream : extraStreams)
								stream.write((line + "\n").getBytes()); //$NON-NLS-1$
						}
						errorContent_.append(line + "\n"); //$NON-NLS-1$
					}
				}
				catch (IOException e)
				{
					e.printStackTrace();
				}
			}
		});
		monitorErrorThread_.start();
	}

	/**
	 * Starts a new thread monitoring stdout.
	 * All "Output" output will be available to be read from <code>getOutputContent()</code>
	 */
	public void startOutputMonitor()
	{
		startOutputMonitor(new OutputStream[0]);
	}

	/**
	 * Starts a new thread monitoring stdout.
	 * All "Output" output will be available to be read from <code>getOutputContent()</code>
	 * @param extraStreams The extra streams array where stdout will be written or null if none
	 */
	public void startOutputMonitor(final OutputStream[] extraStreams)
	{
		monitorOutputThread_ = new Thread(new Runnable() {
			@Override
			public void run()
			{
				try
				{
					String line = ""; //$NON-NLS-1$
					while((line = readOutputLine()) != null)
					{
						if (extraStreams != null)
						{
							for(OutputStream stream : extraStreams)
								stream.write((line + "\n").getBytes()); //$NON-NLS-1$
						}
						outputContent_.append(line + "\n"); //$NON-NLS-1$
					}

				}
				catch (IOException e)
				{
					e.printStackTrace();
				}
			}
		});
		monitorOutputThread_.start();
	}

	/**
	 * Gets the content (as String) of the stderr,
	 * if the caller started "startErrorMonitor"
	 * @return
	 */
	public String getErrorContent()
	{
		return errorContent_.toString();
	}

	/**
	 * Gets the content (as String) of the stdout,
	 * if the caller started "startOutputMonitor"
	 * @return
	 */
	public String getOutputContent()
	{
		return outputContent_.toString();
	}

	/**
	 * Returns the OutputStream
	 * @return
	 */
	public OutputStream getStdin()
	{
		if (process_ == null)
			return null;

		return process_.getOutputStream();
	}

	/**
	 * Returns the InputStream
	 * @return
	 */
	public InputStream getStdout()
	{
		if (process_ == null)
			return null;

		return process_.getInputStream();
	}

	/**
	 * Returns the ErrorStream
	 * @return
	 */
	public InputStream getStderr()
	{
		if (process_ == null)
			return null;

		return process_.getErrorStream();
	}

	/**
	 * Returns true if the process ended
	 * @return
	 */
	public boolean processEnded()
	{
		try
		{
			if (process_ != null)
				process_.exitValue();
			else
				return false;
		} catch (IllegalThreadStateException e)
		{
			// the process hasn't exited
			return false;
		}
		return true;
	}

	/**
	 * Kills the current opened tool. No effect is tool is already killed
	 *
	 * @param waitForKilling true to wait until the process is killed, so when the call
	 * returns the process will be already finished (it has "exitValue")
	 */
	public void kill(boolean waitForKilling)
	{
		if (process_ != null)
		{
			process_.destroy();
			if (waitForKilling)
			{
				try
				{
					process_.waitFor();
				} catch (InterruptedException e)
				{
				}
			}
			monitorErrorThread_.interrupt();
			monitorOutputThread_.interrupt();
		}
	}

	/**
	 * Launches the specified tool, with the specified argument list.
	 * The caller returns immediatelly
	 *
	 * @param fileName the full path to the executable to be launched
	 * @param args the arguments list
	 * @param stdout An array of outputstreams where the stdout from the tool will be written
	 * @param stderr An array of outputstreams where the stderr from the tool will be written
	 * @return
	 */
	public static ExternalToolInvoker launchTool(final String fileName, final List<String> args,
			final OutputStream[] stdout, final OutputStream[] stderr)
	{
		final ExternalToolInvoker toolInvoker = new ExternalToolInvoker(fileName, args);
		toolInvoker.runTool();
		Thread outputStreamThread = new Thread(new Runnable(){
			@Override
			public void run()
			{
				try
				{
					String line = ""; //$NON-NLS-1$
					while((line = toolInvoker.readOutputLine()) != null)
					{
						for (OutputStream out : stdout)
						{
							out.write((line + "\n").getBytes()); //$NON-NLS-1$
						}
					}
				} catch (IOException e)
				{
					Logger.getInstance().logException(e);
				}
			}
		});
		Thread errorStreamThread = new Thread(new Runnable(){
			@Override
			public void run()
			{
				try
				{
					String line = ""; //$NON-NLS-1$
					while((line = toolInvoker.readErrorLine()) != null)
					{
						for (OutputStream err : stderr)
						{
							err.write((line + "\n").getBytes()); //$NON-NLS-1$
						}
					}
				} catch (IOException e)
				{
					Logger.getInstance().logException(e);
				}
			}
		});
		outputStreamThread.start();
		errorStreamThread.start();
		return toolInvoker;
	}
}
