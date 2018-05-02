function new_t_data = change_trajectory_format(t_data)
wgs84 = wgs84Ellipsoid('meters');
new_t_data = t_data;
disp(t_data)

%LLH -> ECF
if strcmp('LLH',t_data.format) == 1
      
    for i = 1 : max(size(t_data.lat))
        llh = [t_data.lat(i) t_data.lon(i) t_data.h(i)];
        xyz = lla2ecef(llh);

        new_t_data.x(i,:) = xyz(1);
        new_t_data.y(i,:) = xyz(2);
        new_t_data.z(i,:) = xyz(3);
    end
    
    field = {'lat', 'lon', 'h'};
    new_t_data = rmfield(new_t_data,field);
    
    new_t_data.format = 'ECEF';
end

%ECEF -> LLH
if strcmp('ECEF',t_data.format) == 1

    [ new_t_data.lat,...
      new_t_data.lon,...
      new_t_data.h ] = ecef2geodetic(wgs84,t_data.x,t_data.y,t_data.z);

    field = ['x'; 'y'; 'z'];
    new_t_data = rmfield(new_t_data,field);
    
    new_t_data.format = 'LLH';
end
end



