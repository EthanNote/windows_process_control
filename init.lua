ps.add('hub', 'python C:/Users/snipe_000/Documents/dev/radar_standalone_app/service/service_hub.py')
ps.add('worker', 'python C:/Users/snipe_000/Documents/dev/radar_standalone_app/service/worker.py')

print('add tasks')

function do_ps(msg)
    action = msg.action
    if action then
        if action == 'add' then
            if msg.name and msg.cmd then
                ps.add(msg.name, msg.cmd)
                return 'add command executed'
            end
        end
        if action == 'run' then
            if msg.name then
                ps.run(msg.name)
                return 'run command executed'
            end
        end
        if action == 'kill' then
            if msg.name then
                ps.kill(msg.name)
                return 'kill command executed'
            end
        end
    end
    return 'invalid command '..cjson.encode(msg)
end

function on_data(msg)
    json = cjson.decode(msg)
    print(json.client)
    if json.client then
        return client.getstate()
    else
        return do_ps(json)
    end
end
